# VAL

[![Build Status](https://dev.azure.com/schlumberger/ai-planning-validation/_apis/build/status/ai-planning-tool-val-CI?branchName=master)](https://dev.azure.com/schlumberger/ai-planning-validation/_build/latest?definitionId=2&branchName=master)

This repository hosts tools for AI Planning plans and planning models.

If all you need is binaries, click the _Azure Pipeline_ link above, open the latest green build navigate to the _Summary_ tab and then download the artifact for your operating system. Binaries are available for Linux, Windows and MacOS.

Please report problems and feature requests

## Authors

* Maria Fox and Derek Long - PDDL2.2 and VAL
* Richard Howey - PDDL2.2 and VAL and Continuous Effects, derived predicates, timed initial literals and LaTeX report in VAL
* Stephen Cresswell - PDDL2.2 parser

## Updates

* July 2019: Change license from [GNU Lesser General Public License, version 3](https://opensource.org/licenses/LGPL-3.0) to [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause)

## Applications

## `analyze`

Analyse a domain and problem file to give you information about type structure, relationship between arguments, overloading of predictates and functions.

## `domainview` - WIP

Turn a domain into a visualisation (LaTeX document)

## `howwhatwhen`

Analyse domain, problem (and plan) identifying decision making strategy of a planner.

## `instantiate`

Ground the domain and problem.

## `instantiationip`

Refined variant of instatiate (not compiled by default, still buggy)

## `parser`

The PDDL parser will find and report errors in PDDL more explicitly than validate.

Parse domain file:

`parser <domainfile>`

Parse domain and problem file:

`parser <domainfile> <problemfile>`

## PinguPlan

Application to create a plan for the [Pingus](https://pingus.seul.org/index.html) application

## PlanRec

Identify which action are achieving precondition of a later action in the plan.

## PlanSeqStep

Scripting of multiple plan execution and extraction of final state.

Use output of PlanToValStep output

## PlanToValStep

This utility takes a plan file as input and writes out a script in the form used by ValStep, all as one output.
This allows you to then run all or part of the plan through ValStep easily.

`PlanToValStep myplan.pddl > myvalstepscript`

## Relax

Auto generation of a relaxed domain. Used to compare relax plan and real plan.

## TIM

Type Inference Mechanism (1998) tells which predicate are single value.

## ToFn

To function uses the TIM analysis to translate the domain from a predicate model to a multi-value model.

## TypeAnalysis

`TypeAnalysis <domainfile> <problemfile>`

The type-checking tool is reasonably robust at finding type errors in your PDDL domain/problem files.
Note that the PDDL parser will find and report errors in PDDL more explicitly.

## Validate

This utility is mainly used by planning engine developers and researchers, but it may be useful to a PDDL modeler as well.
You may get to a situation, when you are not sure why the planning engine is _not_ generating the plan you are expecting.
In that case, you could handcraft the plan you expect to get and ask the Validator whether it is valid.
It usually tells you where you are missing a pre-condition, or where you are violating an over all constraint.

Validate has many command line options, but the most important first few are:

`Validate -t <number> -v <domainfile> <problemfile> <planfile....>`

Multiple plan files can be handled together. The `-t` flag allows the value of epsilon to be set.
The default value is _0.01_, but _0.001_ is a good value to use for most planners.
Actions separated by epsilon or less are treated as simultaneous by Validate.
The `-v` switch is the verbose mode.

Syntax for validating your domain model:

`Validate domain.pddl`

Syntax for validating your domain model and problem file:

`Validate domain.pddl problem.pddl`

Syntax for getting more insights into what is happening during your plan (theoretical) execution or validating a hand-coded plan:

`Validate -v -t 0.001 domain.pddl problem.pddl plan.txt`

Syntax for generating LaTeX file that visualizes the plan and changes of function values throughout the plan.

`Validate -l -f report -t 0.001 domain.pddl problem.pddl plan.txt`

The above will generate report.tex (the .tex extension is automatically added, so need not be placed on the command line).

To render the .tex file as a .pdf, download [MiKTeX](https://miktex.org/download). After you install it, its bin folder is added to your %path% and you may simply use it from a command line this way:

`pdflatex -synctex=1 -interaction=nonstopmode report.tex`

The first run will pop up a dialog asking for your consent to download additional packages needed by this .tex file. After that you should be able to just run

`pdflatex report.tex`

If you want to do the .pdf generation painlessly, you can just install the [LaTeX Preview extension for VSCode](https://marketplace.visualstudio.com/items?itemName=ajshort.latex-preview).

## ValStep

This tool is run with command line:

`ValStep [-i <input_file>] <domain> <problem>`

It gives a prompt "`?`" and expects one of the valid commands:

* `start <action> @ <time>`
* `end <id> @ <time>`
* `x`
* `w <filename>`
* `q`
* `e <line of text to echo>`

These commands do the following:

`start <action> @ <time>` queues an instantaneous action or a start of a durative action for execution at the given time (which should be higher/later than previous one). ValStep reports a number which is the action id code that must be used when queuing the corresponding end using `end <id> @ <time>`.
The command `x` causes all enqueued actions to be executed. Note that the order in which action starts and ends are added is not important – it is the time that they are specified to occur that determines when the execution will simulate them happening. Also note that `x` will only step forward to the next time at which something is specified to happen. So here is an example:

Consider this short temporal PDDL plan:

```pddl
0.00100: (boil-water) [64.00100]
61.00100: (pump-boiling-water) [3.00000]
```

```shell
ValStep.exe domain.pddl problem.pddl
?
```

Queue first action start:

```shell
? start boil-water @ 0.001
```

ValStep responds:

```shell
Posted action 1
```

Therefore _1_ is the ID of the `boil-water` action.

Execute the action and evaluate the state delta by passing the `x` command:

```shell
? x
```

ValStep responds:

```shell
Seeing 1 changed lits
boiling - now true
```

Predicate `boiling` is now `true`.

Similarly we queue and execute the second action start:

```shell
? start pump-boiling-water @ 61.001
Posted action 2
? x
Seeing 2 changed lits
boiling - now true
pumping - now true
water-temperature - now 96
```

Although this action only changes 2 literals/fluents, ValStep reports all changes since the initial state, including the already establish `boiling`==`true`.

Then we queue and execute the end of the `pump-boiling-water` using the ID `2`:

```shell
? end 2 @ 64.001
? x
Seeing 2 changed lits
boiling - now true
pumping - now false
cup-level - now 30
water-temperature - now 90
```

Last, we close the `boil-water` action:

```shell
? end 1 @ 64.002
? x
Seeing 2 changed lits
boiling - now false
pumping - now false
cup-level - now 30
water-temperature - now 90.001
```

The final state is therefore everything in the initial state plus

```shell
boiling - now false
pumping - now false
cup-level - now 30
water-temperature - now 90.001
```

In case multiple actions start at the same time, all shall be enqueued before triggering their execution using `x`.

The command `e` echoes the remaineder of the line of input.  This allows the user to annotate the ValStep output as they wish.  For example:

```shell
? e Finished pumping boiling water.
Finished pumping boiling water.
```

The command `q` quits the tool, causing it to print the current state as a PDDL problem file to the console output. If you wish to record the state before exiting, use the command `w <file>` writes the current state (and goal) as a PDDL problem file into the indicated file.

Note that the problem file written out would not replicate any _outstanding_ timed-initial effects from the original problem file. This feature is currently missing.

## ValueSeq

This tool can be used to extract the values of particular numeric state values in the course of a plan. It is used as follows:

`ValueSeq <domain> <problem> <plan> [<function>*] [REMOVE <tag>*]`

where the function expressions are state variables and the `tag`s are strings that if included in an action name, the action is removed from the output (for brevity). The square brackets indicate optional arguments and the `*` symbol indicates arguments that can appear any number of times.

An example usage:

```shell
ValueSeq domain.pddl problem_boil_and_pour1.pddl problem_boil_and_pour1.plan water-temperature
```

... outputs ...

```csv
boil-water, 35, 90.001
pump-boiling-water, 35, 90
```

Using the `-t` switch:

```shell
ValueSeq.exe -t domain.pddl problem_boil_and_pour1.pddl problem_boil_and_pour1.plan water-temperature
```

... outputs ...

```csv
0.001, boil-water, 35, 90.001
61.001, pump-boiling-water, 35, 90
```

The output shows changes in function values before and after an action execution. However, if you look up the action start time and duration in the plan file, you can reconstruct a temporal sequence and plot it. Mind the cases, where the same action appears in the plan more than once.

Using the `-T` switch:

```shell
ValueSeq.exe -T domain.pddl problem_boil_and_pour1.pddl problem_boil_and_pour1.plan water-temperature
```

... outputs ...

```csv
water-temperature
0, 35
0.001, 35
61.001, 96
64.001, 90
64.002, 90.001
```

This output may be directly pasted into a spreadsheet program e.g. Excel to render a chart.


## [Libraries](libraries/README.md)

## How to compile VAL

### Windows

* Requirements:
  * [CMake](https://cmake.org/)
    * Include cmake in path for all users
  * [MinGW-w64](https://sourceforge.net/projects/mingw)
    * Install packages: mingw32-base, mingw32-gcc-g++
  * [Strawberry Perl](http://strawberryperl.com/) (required for GCC)
  * [LLVM](http://releases.llvm.org)
  * [Doxygen](http://www.doxygen.nl/download.html)

* IDE:
  * [Visual Studio Code](https://code.visualstudio.com/)
    * Extensions:
      * [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
      * [CMake tools](https://marketplace.visualstudio.com/items?itemName=vector-of-bool.cmake-tools)
      * [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
      * [C++ Intellisense](https://marketplace.visualstudio.com/items?itemName=austin.code-gnu-global)
      * [Clang-Format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format)
    * Warning: You may require to restart your machine to be sure cmake, and gcc tools are identified by Visual Studio Code.

* Scripts
  * [scripts/windows/build_win64_mingw.bat](scripts/windows/build_win64_mingw.bat): Build scripts for windows 64 bits using MinGW
    * Binaries can be found in `build/win64/mingw/Release|Debug/bin`
  * [scripts/windows/build_win64_eclipse_mingw.bat](scripts/windows/build_win64_eclipse_mingw.bat): Build scripts for windows 64 bits using MinGW and creating eclipse project definition.
    * Binaries can be found in `build/win64/eclipse_mingw/Release|Debug/bin`
    * Project can be imported in eclipse following the [instructions](https://www.mantidproject.org/Setting_up_Eclipse_projects_with_CMake).
  * [scripts/windows/build_win64_vs.bat](scripts/windows/build_win64_vs.bat): Build scripts for windows 64 bits using Visual Studio Compiler
    * Binaries can be found in `build/win64/vs/Release|Debug/bin`
  * [scripts/windows/setup_dlfcn-win32.bat](scripts/windows/setup_dlfcn-win32.bat): Build scripts to build dlfcn-win32 library for windows
    * Binaries can be found in `externals/dlfcn-win32/`
  * [scripts/windows/format.bat](scripts/windows/format.bat): Scripts to format the code
  * [scripts/windows/documentation.bat](scripts/windows/documentation.bat): Generate documentation

### Linux

* Requirements:
  * Debian packages: cmake make g++ mingw-w64 flex bison
    * mingw-w64: For cross-compiling
    * flex, bison: For parser code regeneration

* IDE:
  * [Visual Studio Code](https://code.visualstudio.com/)

* Scripts
  * [scripts/linux/build_linux64.sh](scripts/linux/build_linux64.sh): Build scripts for linux 64 bits
    * Binaries can be found in `build/linux64/Release|Debug/bin`
  * [scripts/linux/build_win32.sh](scripts/linux/build_win32.sh): Build scripts for windows 32 bits
    * Binaries can be found in `build/linux_win32/Release|Debug/bin`
  * [scripts/linux/build_win64.sh](scripts/linux/build_win64.sh): Build scripts for windows 64 bits
    * Binaries can be found in `build/linux_win64/Release|Debug/bin`
  * [scripts/linux/documentation.sh](scripts/linux/documentation.sh): Generate documentation
  * [scripts/linux/format.sh](scripts/linux/format.sh): Scripts to format the code
  * [scripts/linux/setup_flex_bison.sh](scripts/linux/setup_flex_bison.sh): Build scripts to generate flex and bison header/implementation files from [libraries/VAL/src/parser/pddl+.l](libraries/VAL/src/parser/pddl+.l) and [libraries/VAL/src/parser/pddl+.y](libraries/VAL/src/parser/pddl+.y)
    * Sources files can be found in [libraries/VAL/src/parser/pddl+.cpp](libraries/VAL/src/parser/pddl+.cpp) and [libraries/VAL/src/parser/pddl+.lex.yy.h](libraries/VAL/src/parser/pddl+.lex.yy.h)

### macOS

* Requirements:
  * Xcode packages: cmake make g++ gcc flex bison
  * Note: easier to simply install Xcode in its entirety; necessary packages automatically downloaded

* IDE:
  * [Visual Studio Code](https://code.visualstudio.com/)

* Scripts
  * Note: all called (internally as well) build and clean scripts must be run through dos2unix
  * (scripts/build_macos_dev.sh): script for native macOS build
    * Binaries can be found in `build/macos64/Release/bin`
  * (scripts/clean_macos_dev.sh): script for cleaning out built binaries

  * (scripts/linux/setup_flex_bison.sh): build script to generate flex and bison header/implementation files from [libraries/VAL/src/parser/pddl+.l](libraries/VAL/src/parser/pddl+.l) and [libraries/VAL/src/parser/pddl+.y](libraries/VAL/src/parser/pddl+.y)
  * Note: flex on macOS currently causing compatibility issues
    * Do not run setup_flex_bison.sh
    * Use current Windows-generated flex header file at [libraries/VAL/src/parser/pddl+.lex.yy.h]
  * Note: bison on macOS running well
    * Edit YACC file at [libraries/VAL/src/parser/pddl+.y] as needed
    * Run bison command from script (bison pddl+.y -o pddl+.cpp) directly through terminal

## How to contribute to VAL

Please submit any defects as [Issues](https://github.com/KCL-Planning/VAL/issues) via GitHub.
If you are missing a feature, report it as an issue, but tag it as `[feature]` please.

We appreciate community contributions to this open sourced code repository. By submitting a pull request you certify that the contribution is your original work, you ensure the contribution is compatible with this repository [license](LICENSE) terms, and you agree (including on behalf of your employer, if applicable) that the community is free to use your contributions.

If you have a summer intern or a post-doc student and need a project for a few weeks or months, pick from the backlog of [Issues](https://github.com/KCL-Planning/VAL/issues), or just address the numerous c++ build warnings and help us modernize the codebase.

