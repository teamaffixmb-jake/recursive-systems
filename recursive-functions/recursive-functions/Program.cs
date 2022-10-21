using recursive_functions;

/// <summary>
/// Input seed file format:
/// FOR EACH LINE:
/// <substate> <equation>
/// </summary>

bool[] initialState = { true, false, false, false, true, false, false, false, false, false };

Random random = new Random();

//RecursiveSystem recursiveSystem = new RecursiveSystem("../../../../../future-mapping/system-0.txt", initialState);
RecursiveSystem recursiveSystem = RecursiveSystem.Random(random, initialState);

for (int i = 0; true; i++)
{
    bool[] state = recursiveSystem.Next();
    Console.WriteLine(i.ToString() + ": " + StateToString(state));
}

//Console.WriteLine("Loaded system, initial state pair: " + recursiveSystem.ToString());

//Console.WriteLine("For how many timesteps would you like to simulate this system?");
//string? unparsedSimulationDuration = Console.ReadLine();

//if (string.IsNullOrEmpty(unparsedSimulationDuration))
//{
//    Console.WriteLine("Empty simulation duration. Failure.");
//    return;
//}

//if (!uint.TryParse(unparsedSimulationDuration, out uint simulationDuration))
//{
//    Console.WriteLine("Failed to parse simulation duration. Failure.");
//    return;
//}

//int simulationDurationDisplayDigits = simulationDuration.ToString().Length;

//Console.WriteLine(0.ToString("D" + simulationDurationDisplayDigits.ToString()) + ": " + StateToString(state));

//for (int i = 0; i < simulationDuration; i++)
//{
//    state = recursiveSystem.Evaluate(state);
//    Console.WriteLine(i.ToString("D" + simulationDurationDisplayDigits.ToString()) + ": " + StateToString(state));
//}

string StateToString(bool[] state)
{
    string result = "";
    foreach (bool value in state)
        result += value ? "1 " : "0 ";
    return result;
}
