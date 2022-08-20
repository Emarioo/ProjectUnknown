/* Implementations of some modules

Multiple definitions of IMPL would include defintions of functions multiple times. Don't do this.
*/

#define ENGONE_LOGGER_IMPL
#include "Engone/Utilities/LoggingModule.h"

#define ENGONE_FILEUTIL_IMPL
#include "Engone/Utilities/FileUtility.h"

#define ENGONE_TIMEUTIL_IMPL
#include "Engone/Utilities/TimeUtility.h"

#define ENGONE_RANDOMUTIL_IMPL
#include "Engone/Utilities/RandomUtility.h"
