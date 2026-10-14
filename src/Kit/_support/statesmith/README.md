# StateSmith Support {#statesmith_support}

\brief KIT library support for using the StateSmith for auto-generated [State Machine](https://github.com/StateSmith/StateSmith) source code.

The KIT library infrastructure provides the `genfsm2.py` script to customize the
output of the StateSmith tool to be specific to the KIT library design model.

**Constraints:**

- Only supports Plant UML for 'drawing' the State Machine diagrams
  - KIT specific configuration is embedded in the Plant UML file
  - Implicit assumption that the developer's IDE provides Plant UML preview
    plugin/feature.
- Only supports the State Smith C++ language output, i.e. `--lang Cpp`
- All actions must be function calls followed by a semi-colon, e.g `myActionA();`
- All guards must be function calls that return a boolean, e.g, `isItTuesday()`

**Extensions:**

- Adds the KIT library copyright info to the files
- Auto-generates virtual methods for all action and guard methods
- Auto-generates an `isIn<State>()` query method for each state in the generated
  `StateId` enum
  - For a composite (non-leaf) state the method walks the parent chain, so it
    also returns true when any of the state's sub-states is active.
  - **These methods are for UNIT TESTING ONLY.** They are not thread safe, and
    the FSM's active state is transiently `ROOT` while a transition is in
    progress - so calling them from an action or guard method returns false for
    every state. Application logic that branches on the FSM's current state
    belongs in the diagram, not in the application code.
  - Composite states require StateSmith's `getParentId()` function, i.e.
    StateSmith >= 0.22 with `SmRunnerSettings.algoBalanced1.outputGetParentIdFunction`
    enabled (the generated Plant UML template sets this). Code generation fails
    with an explicit error if the diagram has composite states and the function
    is missing.
- Preserves the diagram's original name casing in the generated
  `stateIdToString()`/`eventIdToString()` methods and in the `isIn<State>()`
  method names (StateSmith uppercases the `StateId`/`EventId` enum symbols)
- Properly supports nested namespaces (for C++11)
- Renames the `start()` method to `startFsm()` so as to not conflict with KIT's
  usage of `start()` naming conventions for drivers.
- Optionally adds an event queue that provides run-to-completion semantics
  - Adds KIT trace statements when processing events from the queue.
- (Re)generates an SVG render of the State Machine diagram alongside the
  `.puml` file.
  - Requires [PlantUML](https://plantuml.com/) (and Java and Graphviz) to be
    installed, e.g. `sudo apt-get install -y plantuml` on Linux, or
    `choco install plantuml` on Windows. Alternately set the `PLANTUML_JAR`
    environment variable to a `plantuml.jar`.
  - If PlantUML is not installed, a note is printed and code generation
    continues normally. **This means the committed SVG can drift from its
    `.puml` source** - re-run the generator on a host that has PlantUML to
    refresh it.
  - Each SVG is named after the diagram's `@startuml <name>` directive (falling
    back to the source file's stem when the directive has no name).
  - The `genfsm2-diagrams` alias runs the SVG generator standalone, i.e. to
    refresh diagrams without running code generation:
    `genfsm2-diagrams [-f|--force] DIR [DIR ...]`

**Usage:**

- The StateSmith tool must be installed on the host system and the `ss.cli`
  executable is in the command path.
- The repo's `env.sh`/`env.bat` script defines the `genfsm2` alias to simplify
  running the `genfsm2.py` script. The `genfsm2-diagrams` alias is also defined
  for running the SVG generator by itself.
- For help on running the script, type: `genfsm2 -h`
- When starting a new State Machine, use `genfsm new MyFsm` to generate the
  Plant UML file (`MyFsm.puml`).
  - The generated Plant UML contains the required KIT configuration and
    instructions on how to modify the configuration.
- The script only needs-to-be/should-be run when the State Machine Plant UML
  file changes.
  - Recommendation is to **not** have your CI build run the `genfsm2.py` script
- The [ContextMyFsm.h](https://github.com/Integerfox/kit.core/blob/main/src/Kit/_support/statesmith/_0test/ContextMyFsm.h)
  provides an example of how an application provides the 'context' for the State 
  Machine diagram, i.e implements the Action and Guard methods.

## Known limitations of StateSmith (as Nov 2025)

- StateSmith does not support UML Junction pseudo states.  However a Choice
  pseudo state can be used where the Choice state has a single outbound
  transition and no guard, or else clause is used on the outbound transition
- There are limitations for StateSmith generated HTML files that simulate the
  State Machine behavior.  See this [wiki page](https://github.com/StateSmith/StateSmith/wiki/Simulator)
  for details
