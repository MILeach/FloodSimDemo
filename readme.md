## Required Software
+ Visual Studio 2017
+ Unreal Engine 4.25.3
+ Python 3

## Build Instructions

1. Build the FLAMEGPU model located in the `FLAMEGPU-development` folder as usual
2. Open the .uproject file located in the `UnrealEngine` folder
3. Click play in the editor. The Unreal Engine will compile the project and run the program. This may take some time on the first build.

## Usage Instructions

### Creating a Simulation
The FLAMEGPU project `iterations` folder should contain one folder for each different simulation. This folder should contain a file called `map.xml` which is the FLAMEGPU initial states file. If you are creating a new simulation, you will need to also copy the `FLAMEGPU-development\examples\FloodPedestrian_SW_stadium\tools\floodMapToOBJ.py` script into the folder and drag and drop the map.xml file onto the script. This will generate the necessary files for the visualisation. This only needs to be done once per project, or when the topology is changed.

### Running a Simulation
When the program is started, a user interface will be present in the top left corner. This consists of three buttons, which start, stop, or quit the simulation program. Beneath this there are two text boxes. The first should contain the folder name containing the map.xml file you wish to use, e.g. `Zone1` or `Default`. The second controls the vertical scale of the topology. Once the desired parameters have been set, click `Start Simulation` to begin the simulation. This will automatically launch the FLAMEGPU model and visualise the results.

If you want to change to a different simulation, just click stop simulation, then update the folder text and click start simulation again. 

### Moving the Camera
The camera can be directed using the WASD, Q and E keys for movement, and the mouse for rotation.
