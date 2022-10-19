using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recursive_functions
{
    public class BinaryOperator
    {
        public virtual bool Evaluate(List<bool> system)
        {
            return false;
        }
    }
    public class Substate : BinaryOperator
    {
        int Index = 0;
        public Substate(int index)
        {
            Index = index;
        }
        public override bool Evaluate(List<bool> system)
        {
            return system[Index];
        }
    }
    public class Not : BinaryOperator
    {
        BinaryOperator InternalOperand;
        public Not(BinaryOperator internalOperand)
        {
            InternalOperand = internalOperand;
        }
        public override bool Evaluate(List<bool> system)
        {
            return !InternalOperand.Evaluate(system);
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
        public override bool Evaluate(List<bool> system)
        {
            return InternalOperand0.Evaluate(system) && InternalOperand1.Evaluate(system);
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
        public override bool Evaluate(List<bool> system)
        {
            return InternalOperand0.Evaluate(system) || InternalOperand1.Evaluate(system);
        }
    }
    public class RecursiveSystem : List<BinaryOperator>
    {
        public RecursiveSystem(string unparsedDefinition)
        {

        }
        public List<bool> Evaluate(List<bool> system)
        {
            List<bool> result = new List<bool>(system.Count);
            for (int i = 0; i < system.Count; i++)
                result[i] = this[i].Evaluate(system);
            return result;
        }
        public List<bool> Evaluate(List<bool> system, int iterations)
        {
            List<bool> result = system;
            for (int i = 0; i < iterations; i++)
                result = Evaluate(result);
            return result;
        }
    }
}
