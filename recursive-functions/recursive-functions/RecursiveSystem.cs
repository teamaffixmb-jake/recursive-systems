using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recursive_functions
{
    public class BinaryOperator
    {
        public virtual bool Evaluate(bool[] state)
        {
            return false;
        }
        public static BinaryOperator Parse(string unparsed)
        {
            // Remove all spaces from the unparsed string.
            unparsed = unparsed.Trim(' ');

            // Very first thing, is try and see if the unparsed thing
            // is just an index operator.
            if (uint.TryParse(unparsed, out uint systemIndex))
            {
                return new SubstateIndex(systemIndex);
            }

            Exception instructionMalformedException = new Exception("Error, malformed binary operator instruction: " + unparsed);
            
            const int BEGIN_OPERATOR_INDICATOR_LENGTH = 2;
            const int   END_OPERATOR_INDICATOR_LENGTH = 1;

            if (unparsed.Length < BEGIN_OPERATOR_INDICATOR_LENGTH + END_OPERATOR_INDICATOR_LENGTH + 1)
                throw instructionMalformedException;

            const string          NOT_INDICATOR = "!(";
            const string          AND_INDICATOR = "&(";
            const string           OR_INDICATOR = "|(";
            const string END_OPERATOR_INDICATOR = ")";
            
            // Get the indicator for which operator should be used.
            string operatorIndicator = unparsed.Substring(0, BEGIN_OPERATOR_INDICATOR_LENGTH);
            string finalCharacter = unparsed.Substring(unparsed.Length - END_OPERATOR_INDICATOR_LENGTH, END_OPERATOR_INDICATOR_LENGTH);
            
            // Get the arguments (contents of the operator).
            string arguments = unparsed.Substring(BEGIN_OPERATOR_INDICATOR_LENGTH, unparsed.Length - BEGIN_OPERATOR_INDICATOR_LENGTH - END_OPERATOR_INDICATOR_LENGTH);

            int argumentsCommaIndex = -1;
            int argumentsParenthesisRank = 0;

            for (int i = 0; i < arguments.Length; i++)
            {
                if (arguments[i] == '(') argumentsParenthesisRank++;
                else if (arguments[i] == ')') argumentsParenthesisRank--;
                else if (argumentsParenthesisRank == 0 && arguments[i] == ',') { argumentsCommaIndex = i; break; }
            }

            string argument0 = "";
            string argument1 = "";

            if (argumentsCommaIndex >= 0)
            {
                argument0 = arguments.Substring(0, argumentsCommaIndex);
                argument1 = arguments.Substring(argumentsCommaIndex + 1, arguments.Length - argument0.Length - 1);
            }

            if (finalCharacter != END_OPERATOR_INDICATOR)
                throw instructionMalformedException;

            switch(operatorIndicator)
            {
                case NOT_INDICATOR:
                    {
                        return new Not(Parse(arguments));
                    }
                case AND_INDICATOR:
                    {
                        return new And(Parse(argument0), Parse(argument1));
                    }
                case OR_INDICATOR:
                    {
                        return new Or(Parse(argument0), Parse(argument1));
                    }
                default:
                    {
                        // An error occured. An unrecognized operator indicator was provided.
                        throw instructionMalformedException;
                    }
            }

        }
    }
    public class SubstateIndex : BinaryOperator
    {
        uint Index = 0;
        public SubstateIndex(uint index)
        {
            Index = index;
        }
        public override bool Evaluate(bool[] state)
        {
            return state[(int)Index];
        }
        public override string ToString()
        {
            return Index.ToString();
        }
    }
    public class Not : BinaryOperator
    {
        BinaryOperator InternalOperand;
        public Not(BinaryOperator internalOperand)
        {
            InternalOperand = internalOperand;
        }
        public override bool Evaluate(bool[] state)
        {
            return !InternalOperand.Evaluate(state);
        }
        public override string ToString()
        {
            return "!(" + InternalOperand.ToString() + ")";
        }
    }
    public class And : BinaryOperator
    {
        BinaryOperator InternalOperand0;
        BinaryOperator InternalOperand1;
        public And(BinaryOperator internalOperand0, BinaryOperator internalOperand1)
        {
            InternalOperand0 = internalOperand0;
            InternalOperand1 = internalOperand1;
        }
        public override bool Evaluate(bool[] state)
        {
            return InternalOperand0.Evaluate(state) && InternalOperand1.Evaluate(state);
        }
        public override string ToString()
        {
            return "&(" + InternalOperand0.ToString() + "," + InternalOperand1.ToString() + ")";
        }
    }
    public class Or : BinaryOperator
    {
        BinaryOperator InternalOperand0;
        BinaryOperator InternalOperand1;
        public Or(BinaryOperator internalOperand0, BinaryOperator internalOperand1)
        {
            InternalOperand0 = internalOperand0;
            InternalOperand1 = internalOperand1;
        }
        public override bool Evaluate(bool[] state)
        {
            return InternalOperand0.Evaluate(state) || InternalOperand1.Evaluate(state);
        }
        public override string ToString()
        {
            return "|(" + InternalOperand0.ToString() + "," + InternalOperand1.ToString() + ")";
        }
    }
    public class RecursiveSystem : List<BinaryOperator>
    {
        public RecursiveSystem(BinaryOperator[] unparsed) :
            base(unparsed)
        {

        }
        public bool[] Evaluate(bool[] state)
        {
            bool[] result = new bool[state.Length];
            for (int i = 0; i < state.Length; i++)
                result[i] = this[i].Evaluate(state);
            return result;
        }
        public bool[] Evaluate(bool[] state, int iterations)
        {
            bool[] result = state;
            for (int i = 0; i < iterations; i++)
                result = Evaluate(result);
            return result;
        }
        public override string ToString()
        {
            string result = "";
            
            foreach (BinaryOperator binaryOperator in this)
                result += binaryOperator.ToString() + " ";
            
            return result;
        }
    }
}
