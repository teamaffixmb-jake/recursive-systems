using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recursive_functions
{
    /// <summary>
    /// An input that gets taken from a previously inputted system state, which may or may not be inverted.
    /// </summary>
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
    /// <summary>
    /// A list of (possibly) inverted binary inputs, which when put through a product, will result in a minterm.
    /// </summary>
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
    /// <summary>
    /// A list of minterms which are included in a minterm sum, so as to produce a binary operator.
    /// </summary>
    public class BinaryOperator : List<Minterm>
    {
        public static BinaryOperator Random(Random random, int systemSize)
        {
            int mintermSumSize = (int)Math.Pow(2, systemSize);
            BinaryOperator result = new BinaryOperator();
            for (int i = 0; i < mintermSumSize; i++)
            {
                if (random.Next(2) == 1)
                    result.Add(new Minterm(systemSize, i));
            }
            return result;
        }
        public BinaryOperator(int systemSize, bool[] mintermSum)
        {
            for (int i = 0; i < mintermSum.Length; i++)
            {
                if (mintermSum[i])
                    // Only add the minterms we need to for computation.
                    Add(new Minterm(systemSize, i));
            }
        }
        public BinaryOperator()
        {

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
    /// <summary>
    /// A class which holds multiple binary operators. Each operator acts on inputs from the previous timestep 
    /// to evaluate the current timestep.
    /// </summary>
    public class RecursiveSystem : List<BinaryOperator>
    {
        private bool[]? state;
        public static RecursiveSystem Random(Random random, bool[] initialState)
        {
            List<BinaryOperator> binaryOperators = new List<BinaryOperator>();
            for (int i = 0; i < initialState.Length; i++)
                binaryOperators.Add(BinaryOperator.Random(random, initialState.Length));
            return new RecursiveSystem(binaryOperators, initialState);

        }
        public RecursiveSystem(string? filePath, bool[] initialState)
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

            // Ensure that the size of the state is valid.
            if (initialState.Length != lines.Length)
                throw new Exception("Error: state length is invalid. State length: " + initialState.Length + ", system size: " + lines.Length);

            // Save the initial state.
            this.state = initialState;

        }
        /// <summary>
        /// Constructor which takes the list of binary operators as well as the initial state of the system.
        /// </summary>
        /// <param name="binaryOperators"></param>
        /// <param name="initialState"></param>
        public RecursiveSystem(List<BinaryOperator> binaryOperators, bool[] initialState) :
            base(binaryOperators)
        {
            this.state = initialState;
        }
        /// <summary>
        /// Evaluates the previous state of the system to produce the current state.
        /// </summary>
        /// <param name="state"></param>
        /// <returns></returns>
        /// <exception cref="Exception"></exception>
        public bool[] Next()
        {
            // Assert the fact that the member variable: state cannot be null.
            if (state == null)
                throw new Exception("Error: this.state was null.");
            
            bool[] result = new bool[state.Length];
            
            // Loop through all binary operators, evaluating them on the previous state.
            for (int i = 0; i < state.Length; i++)
                result[i] = this[i].Evaluate(state);

            // Save the current state for the next time calling Next().
            state = result;
            
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
