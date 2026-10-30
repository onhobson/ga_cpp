~~Write in-code documentation.~~   
Write external documentation.

C++ Concepts/Constraints.   
Error checking on user inputs. (Anything not handled by Concepts)

Expand GTest to cover rest of library.

Add more operator functions.  
Add pre-defined operator bundles. (Can be used in place of user built Operators object in GA constructor.)

Expand config options.

Track inter-run metrics.   
Create Result object to return result + metrics.
Alternatively, use callback function to track metrics? Supply a pre-defined metric-collecting callback for optional use?

Optimization
 - Parallelism
 - Templated eval func storage (inline capability?)
 - Find unnecessary copying

Callback function?

GA currently always shuffles a base sequence to create the initial population. This might not work for all sequence types (assumes LegacyRandomAccessIterator)   
Allow user to initialize GA with a pre-populated full population?
Alternatively, supply predefined functions for initializing population, but allow user to supply custom?


NEED SKILLS:
 - Revisit rvalue/lvalue/etc
 - - std::move
 - - std::forward / Forward reference
 - Actually understand CMake
