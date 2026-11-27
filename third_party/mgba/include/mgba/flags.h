/* TinyOS build flags for the vendored mGBA core.
 *
 * This replaces CMake's generated include/mgba/flags.h. The core is built
 * without OpenGL, debuggers, scripting or optional libraries. Both emulation
 * cores are enabled; the frontend chooses one by file extension.
 */
#ifndef FLAGS_H
#define FLAGS_H

#define M_CORE_GBA
#define M_CORE_GB

#endif
