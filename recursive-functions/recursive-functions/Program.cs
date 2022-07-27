
Random g_random = new Random();

BinaryOperand l_x0_recursive_formula =
        new Or(
            new BinaryOperand("x0(0)"),
            new BinaryOperand("x1(0)"));

BinaryOperand l_x1_recursive_formula =
        new And(
            new BinaryOperand("x0(0)"),
            new BinaryOperand("x1(0)"));

Console.WriteLine(l_x0_recursive_formula.ToString(0));
Console.WriteLine(l_x1_recursive_formula.ToString(0));

BinaryOperand l_next_x0 = l_x0_recursive_formula.Substitute(
    new List<Substitution> { 
        new Substitution("x0(0)", l_x0_recursive_formula),
        new Substitution("x1(0)", l_x1_recursive_formula)
    });

BinaryOperand l_next_x1 = l_x1_recursive_formula.Substitute(
    new List<Substitution> {
        new Substitution("x0(0)", l_x0_recursive_formula),
        new Substitution("x1(0)", l_x1_recursive_formula)
    });

Console.WriteLine(l_next_x0.ToString(0));
Console.WriteLine(l_next_x1.ToString(0));

struct Substitution
{
    public string m_identifier;
    public BinaryOperand m_substitute;
    public Substitution(string a_identifier, BinaryOperand a_substitute)
    {
        m_identifier = a_identifier;
        m_substitute = a_substitute;
    }
}
class BinaryOperand
{
    public string m_identifier = "";
    public BinaryOperand()
    {

    }
    public BinaryOperand(string a_identifier)
    {
        m_identifier = a_identifier;
    }
    public virtual string ToString(int a_tab_index)
    {
        return Tabs(a_tab_index) + m_identifier;
    }
    public virtual BinaryOperand Substitute(List<Substitution> a_substitutions)
    {
        foreach (Substitution l_substitution in a_substitutions)
        {
            if (l_substitution.m_identifier == m_identifier)
                return l_substitution.m_substitute;
        }
        return this;
    }
    protected static string Tabs(int a_tabs)
    {
        string l_result = "";
        for (int i = 0; i < a_tabs; i++)
            l_result += "    ";
        return l_result;
    }

}
class And : BinaryOperand
{
    public BinaryOperand m_operand_0;
    public BinaryOperand m_operand_1;
    public And(BinaryOperand a_operand_0, BinaryOperand a_operand_1)
    {
        m_operand_0 = a_operand_0;
        m_operand_1 = a_operand_1;
    }
    public override string ToString(int a_tab_index)
    {
        return 
            Tabs(a_tab_index) +
            "&&(\n" +
            m_operand_0.ToString(a_tab_index + 1) +
            ",\n" +
            m_operand_1.ToString(a_tab_index + 1) + "\n" +
            Tabs(a_tab_index) + ")";
    }
    public override BinaryOperand Substitute(List<Substitution> a_substitutions)
    {
        return new And(m_operand_0.Substitute(a_substitutions), m_operand_1.Substitute(a_substitutions));
    }
}
class Or : BinaryOperand
{
    public BinaryOperand m_operand_0;
    public BinaryOperand m_operand_1;
    public Or(BinaryOperand a_operand_0, BinaryOperand a_operand_1) :
        base("(" + a_operand_0.ToString() + " || " + a_operand_1.ToString() + ")")
    {
        m_operand_0 = a_operand_0;
        m_operand_1 = a_operand_1;
    }
    public override string ToString(int a_tab_index)
    {
        return
            Tabs(a_tab_index) +
            "||(\n" +
            m_operand_0.ToString(a_tab_index + 1) +
            ",\n" +
            m_operand_1.ToString(a_tab_index + 1) + "\n" +
            Tabs(a_tab_index) + ")";
    }
    public override BinaryOperand Substitute(List<Substitution> a_substitutions)
    {
        return new Or(m_operand_0.Substitute(a_substitutions), m_operand_1.Substitute(a_substitutions));
    }
}
class Not: BinaryOperand
{
    public BinaryOperand m_operand_0;
    public Not(BinaryOperand a_operand_0) :
        base("!" + a_operand_0.ToString())
    {
        m_operand_0 = a_operand_0;
    }
    public override string ToString(int a_tab_index)
    {
        return
            Tabs(a_tab_index) + "!" + m_operand_0.ToString(0);
    }
    public override BinaryOperand Substitute(List<Substitution> a_substitutions)
    {
        return new Not(m_operand_0.Substitute(a_substitutions));
    }
}
