using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recursive_functions
{
    public class BinaryInputWithPossibleInversion
    {
        public int index;
        public bool performInversion;
        public BinaryInputWithPossibleInversion(int index, bool performInversion)
        {
            this.index = index;
            this.performInversion = performInversion;
        }
        public bool Evaluate(bool[] state)
        {
            // Perform XOR between the inversion signal and the input substate.
            return performInversion != state[index];
        }
    }
    public class Minterm : List<BinaryInputWithPossibleInversion>
    {
        public Minterm(int systemSize, int mintermIndex)
        {
            for (int i = 0; i < systemSize; i++)
            {
                // Perform a bit shift before AND-ing the result with mintermIndex, so as to reveal
                // whether or not an inversion should occur on the given binary input.
                bool performInversion = (mintermIndex & ((int)Math.Pow(2, systemSize - 1) >> i)) == 0;
                // Construct binaryinput
                Add(new BinaryInputWithPossibleInversion(i, performInversion));
            }
        }
        public bool Evaluate(bool[] state)
        {
            for (int i = 0; i < Count; i++)
            {
                if (!this[i].Evaluate(state))
                    // If EVER the evaluation of a binary input is false, we can return false, seeing as 
                    // a minterm is made up of repeated ANDs of the (inverted)/(not inverted) binary inputs.
                    return false;
            }
            return true;
        }
    }
    public class BinaryOperator : List<Minterm>
    {
        public BinaryOperator(int systemSize, bool[] mintermSum)
        {
            for (int i = 0; i < mintermSum.Length; i++)
            {
                if (mintermSum[i])
                    // Only add the minterms we need to for computation.
                    Add(new Minterm(systemSize, i));
            }
        }
        public bool Evaluate(bool[] state)
        {
            for (int i = 0; i < Count; i++)
            {
                if (this[i].Evaluate(state))
                    // Return true if ONE minterm evaluates to true because we are OR-ing all of the minterms
                    return true;
            }
            return false;
        }
    }
    public class RecursiveSystem : List<BinaryOperator>
    {
        public RecursiveSystem(string? filePath)
        {
            // Do some sanity check on the file path.
            if (string.IsNullOrEmpty(filePath))
            {
                Console.WriteLine("Empty system path string. Failure.");
                return;
            }

            // Import all lines from the file at once.
            string[] lines = File.ReadAllLines(filePath);

            // For every line in the file, construct a binary operator based off of the
            // minterm sum represented in that line.
            foreach (string line in lines)
            {
                // Make sure that the binary representation of the minterm sum makes sense
                // based off of the known size of the system
                if (Math.Log2(line.Length) != lines.Length)
                    throw new Exception("Error: incorrect minterm sum binary string length.");
                Add(new BinaryOperator(lines.Length, BoolArrayFromBinaryString(line)));
            }

        }
        public bool[] Evaluate(bool[] state)
        {
            if (state.Length != Count)
                throw new Exception("State size was invalid given the system size. State size: " + state.Length + ", system size: " + Count);
            bool[] result = new bool[state.Length];
            for (int i = 0; i < state.Length; i++)
                result[i] = this[i].Evaluate(state);
            return result;
        }
        private static bool[] BoolArrayFromBinaryString(string state)
        {
            bool[] result = new bool[state.Length];
            for (int i = 0; i < result.Length; i++)
                result[i] = state[i] == '1';
            return result;
        }
    }
}
