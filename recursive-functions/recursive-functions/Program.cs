using recursive_functions;

/// <summary>
/// Input seed file format:
/// FOR EACH LINE:
/// <substate> <equation>
/// </summary>

while(true)
{
    Console.WriteLine("Enter a seed file path.");

    string? seedFilePath = Console.ReadLine();

    if (string.IsNullOrEmpty(seedFilePath))
    {
        Console.WriteLine("Empty system path string. Failure.");
        return;
    }

    string unparsedSeed = File.ReadAllText(seedFilePath);
    string[] unparsedSeedSplitByNewline = unparsedSeed.Split("\r\n");
    bool[] state = new bool[unparsedSeedSplitByNewline.Length];
    BinaryOperator[] binaryOperators = new BinaryOperator[unparsedSeedSplitByNewline.Length];

    for (int i = 0; i < unparsedSeedSplitByNewline.Length; i++)
    {
        string[] substateEquationPair = unparsedSeedSplitByNewline[i].Split(" ");
        state[i] = substateEquationPair[0] == "1";
        binaryOperators[i] = BinaryOperator.Parse(substateEquationPair[1]);
    }

    RecursiveSystem recursiveSystem = new RecursiveSystem(binaryOperators);
    Console.WriteLine("Loaded system, initial state pair: " + recursiveSystem.ToString());

    Console.WriteLine("For how many timesteps would you like to simulate this system?");
    string? unparsedSimulationDuration = Console.ReadLine();

    if (string.IsNullOrEmpty(unparsedSimulationDuration))
    {
        Console.WriteLine("Empty simulation duration. Failure.");
        return;
    }

    if (!uint.TryParse(unparsedSimulationDuration, out uint simulationDuration))
    {
        Console.WriteLine("Failed to parse simulation duration. Failure.");
        return;
    }

    int simulationDurationDisplayDigits = simulationDuration.ToString().Length;

    Console.WriteLine(0.ToString("D" + simulationDurationDisplayDigits.ToString()) + ": " + StateToString(state));

    for (int i = 0; i < simulationDuration; i++)
    {
        state = recursiveSystem.Evaluate(state);
        Console.WriteLine(i.ToString("D" + simulationDurationDisplayDigits.ToString()) + ": " + StateToString(state));
    }

}

string StateToString(bool[] state)
{
    string result = "";
    foreach (bool value in state)
        result += value ? "1 " : "0 ";
    return result;
}
