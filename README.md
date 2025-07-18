# VAL

> [!NOTE]
> This repository is a cleaned copy of the main [KCL VAL project](https://github.com/KCL-Planning/VAL). It removes many unnecessary contents and binaries, drops Windows support, and features a much-simplified structure and CMake files to better support compilation on Mac and Linux systems. It also makes it possible to build VAL with Docker, for improved compatibility.
> 
> I don't plan (pun intended) to accept issues or comments because I am NOT the author of the `.h` and `.cpp` files and all the credits go to the authors of the original VAL.

[![Build Status](https://dev.azure.com/schlumberger/ai-planning-validation/_apis/build/status/ai-planning-tool-val-CI?branchName=master)](https://dev.azure.com/schlumberger/ai-planning-validation/_build/latest?definitionId=2&branchName=master)

This repository hosts tools for AI Planning plans and planning models validation and analysis.

## Authors

* Maria Fox and Derek Long - PDDL2.2 and VAL
* Richard Howey - PDDL2.2 and VAL and Continuous Effects, derived predicates, timed initial literals and LaTeX report in VAL
* Stephen Cresswell - PDDL2.2 parser

## Updates

* July 2019: Change license from [GNU Lesser General Public License, version 3](https://opensource.org/licenses/LGPL-3.0) to [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause)

## How to Build

### Local Build (macOS, Linux)

We follow the standard CMake building process. First create a `build` folder then run CMake on it:

```sh
mkdir build
cd build
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
make -j
```

#### Requirements

**Linux:**
* Debian packages: `cmake make g++ flex bison`

**macOS:**
* Xcode packages: `cmake make g++ gcc flex bison`
* Note: easier to simply install Xcode in its entirety; necessary packages automatically downloaded

#### Build Output

The executables will be built inside the folder `build/bin`, alongside the `libVAL.{dylib, so}` depending if you compile on Mac or Linux:

- `analyse`
- `domainview`
- `howwhatwhen`
- `instantiate`
- `parser`
- `pinguplan`
- `planrec`
- `planseqstep`
- `plantovalstep`
- `relax`
- `tim-main`
- `tofn`
- `typeanalysis`
- `validate`
- `valstep`
- `valueseq`

### Docker Build

You can build and run VAL applications using Docker for improved compatibility and ease of setup:

#### 1. Build the Docker image

```sh
# From the root of the repository
docker build -t val .
```

#### 2. Run a VAL application

```sh
docker run --rm -v $(pwd):/workspace val <application> <args>
```

- Replace `<application>` with the name of the VAL executable you want to run
- Replace `<args>` with the arguments you would normally pass to the application
- The `-v $(pwd):/workspace` option mounts your current directory into the container

#### Example: Validate a plan

```sh
docker run --rm -v $(pwd):/workspace val validate domain.pddl problem.pddl plan.txt
```

If all you need is binaries, click the _Azure Pipeline_ link above, open the latest green build navigate to the _Summary_ tab and then download the artifact for your operating system. Binaries are available for Linux and MacOS.

## Applications

### `analyse`

Analyse a domain and problem file to give you information about type structure, relationship between arguments, overloading of predicates and functions.

**Usage:**
```sh
analyse <domainfile> <problemfile>
```

### `domainview`

Turn a domain into a visualisation (LaTeX document). This is currently a work in progress.

**Usage:**
```sh
domainview <domainfile>
```

### `howwhatwhen`

Analyse domain, problem (and plan) identifying decision making strategy of a planner.

**Usage:**
```sh
howwhatwhen <domainfile> <problemfile> [<planfile>]
```

### `instantiate`

Ground the domain and problem, converting all variables to specific instances.

**Usage:**
```sh
instantiate <domainfile> <problemfile>
```

### `parser`

The PDDL parser will find and report errors in PDDL more explicitly than validate.

**Usage:**
```sh
# Parse domain file only
parser <domainfile>

# Parse domain and problem file
parser <domainfile> <problemfile>
```

### `pinguplan`

Application to create a plan for the [Pingus](https://pingus.seul.org/index.html) application.

### `planrec`

Identify which actions are achieving preconditions of a later action in the plan.

**Usage:**
```sh
planrec <domainfile> <problemfile> <planfile>
```

### `planseqstep`

Scripting of multiple plan execution and extraction of final state. Use output of `plantovalstep`.

**Usage:**
```sh
planseqstep <script_file>
```

### `plantovalstep`

This utility takes a plan file as input and writes out a script in the form used by `valstep`. This allows you to then run all or part of the plan through `valstep` easily.

**Usage:**
```sh
plantovalstep <planfile> > <valstep_script>
```

**Example:**
```sh
plantovalstep myplan.pddl > myvalstepscript
```

### `relax`

Auto generation of a relaxed domain. Used to compare relaxed plan and real plan.

**Usage:**
```sh
relax <domainfile> <problemfile>
```

### `tim-main`

Type Inference Mechanism (1998) tells which predicates are single value.

**Usage:**
```sh
tim-main <domainfile> <problemfile>
```

### `tofn`

To function uses the TIM analysis to translate the domain from a predicate model to a multi-value model.

**Usage:**
```sh
tofn <domainfile> <problemfile>
```

### `typeanalysis`

The type-checking tool is reasonably robust at finding type errors in your PDDL domain/problem files. Note that the PDDL parser will find and report errors in PDDL more explicitly.

**Usage:**
```sh
typeanalysis <domainfile> <problemfile>
```

### `validate`

This utility is mainly used by planning engine developers and researchers, but it may be useful to a PDDL modeler as well. You may get to a situation when you are not sure why the planning engine is _not_ generating the plan you are expecting. In that case, you could handcraft the plan you expect to get and ask the Validator whether it is valid. It usually tells you where you are missing a pre-condition, or where you are violating an overall constraint.

**Basic Usage:**
```sh
# Validate domain model only
validate <domainfile>

# Validate domain model and problem file
validate <domainfile> <problemfile>

# Validate a plan with verbose output
validate -v -t 0.001 <domainfile> <problemfile> <planfile>

# Generate LaTeX report
validate -l -f report -t 0.001 <domainfile> <problemfile> <planfile>
```

**Options:**
- `-t <number>`: Set epsilon value (default 0.01, recommended 0.001)
- `-v`: Verbose mode
- `-l`: Generate LaTeX output
- `-f <filename>`: Specify output filename for LaTeX report

Multiple plan files can be handled together. The `-t` flag allows the value of epsilon to be set. Actions separated by epsilon or less are treated as simultaneous by Validate.

**LaTeX Report Generation:**
The `-l` flag generates a LaTeX file that visualizes the plan and changes of function values throughout the plan. To render the .tex file as a .pdf:

```sh
pdflatex -synctex=1 -interaction=nonstopmode report.tex
```

### `valstep`

Interactive tool for step-by-step plan execution and state inspection.

**Usage:**
```sh
valstep [-i <input_file>] <domainfile> <problemfile>
```

**Commands:**
- `start <action> @ <time>`: Queue an action start
- `end <id> @ <time>`: Queue an action end using the ID from start
- `x`: Execute all enqueued actions
- `w <filename>`: Write current state to file
- `e <text>`: Echo text (for annotation)
- `q`: Quit and print current state

**Example Session:**
```shell
? start boil-water @ 0.001
Posted action 1
? x
Seeing 1 changed lits
boiling - now true
? end 1 @ 64.002
? x
Seeing 2 changed lits
boiling - now false
```

### `valueseq`

Extract the values of particular numeric state variables throughout plan execution.

**Usage:**
```sh
valueseq <domainfile> <problemfile> <planfile> [<function>*] [REMOVE <tag>*]
```

**Options:**
- `-t`: Include timestamps in output
- `-T`: Generate time-series format suitable for spreadsheets

**Examples:**
```sh
# Basic usage
valueseq domain.pddl problem.pddl plan.txt water-temperature

# With timestamps
valueseq -t domain.pddl problem.pddl plan.txt water-temperature

# Time-series format
valueseq -T domain.pddl problem.pddl plan.txt water-temperature
```

The output shows changes in function values before and after action execution. The `-T` switch produces output that can be directly pasted into spreadsheet programs like Excel to render charts.

## [Libraries](libraries/README.md)

## How to Contribute

Please submit any defects as [Issues](https://github.com/KCL-Planning/VAL/issues) via GitHub. If you are missing a feature, report it as an issue, but tag it as `[feature]` please.

We appreciate community contributions to this open sourced code repository. By submitting a pull request you certify that the contribution is your original work, you ensure the contribution is compatible with this repository [license](LICENSE) terms, and you agree (including on behalf of your employer, if applicable) that the community is free to use your contributions.

If you have a summer intern or a post-doc student and need a project for a few weeks or months, pick from the backlog of [Issues](https://github.com/KCL-Planning/VAL/issues), or just address the numerous C++ build warnings and help us modernize the codebase.

