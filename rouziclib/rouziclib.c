/* Make a .c file in your project containing an include to the header file you must 
 * create in your project and an include to this file, rouziclib.c, like this:

#include "rl.h"

#include <rouziclib/rouziclib.c>

****************/

// C files

#include "general/xyz.c"
#include "general/time.c"
#include "general/estimates.c"
#include "general/mouse.c"
#include "general/keyboard.c"
#include "general/noop.c"
#include "general/hashing.c"
#include "general/threading.c"
#include "general/intrinsics.c"
#include "general/crash_dump.c"
#include "general/audio.c"
#include "general/midi.c"

#include "memory/alloc.c"
#include "memory/fill.c"
#include "memory/swap.c"
#include "memory/circular_buffer.c"
#include "memory/generic_buffer.c"
#include "memory/comparison.c"

#include "geometry/rect.c"
#include "geometry/intersection.c"
#include "geometry/rotation.c"
#include "geometry/fit.c"
#include "geometry/distance.c"
#include "geometry/bezier.c"
#include "geometry/misc.c"
#include "math/functions.c"
#include "math/cpu_functions.c"
#include "math/ieee754.c"
#include "math/double_double_arithmetic.c"
#include "math/rand.c"
#include "math/dsp.c"
#include "math/dct.c"
#include "math/matrix.c"
#include "math/physics.c"
#include "math/debug.c"
#include "math/polynomials.c"
#include "math/polynomials_mpfr.c"
#include "math/polynomial_signal.c"
#include "math/gaussian_polygon.c"
#include "approximations/fixed_point.c"
#include "approximations/fast_float.c"
#include "approximations/fast_cos.c"
#include "approximations/high_prec.c"
#include "approximations/simd.c"

#include "graphics/graphics.c"
#include "graphics/pixel_conv.c"
#include "graphics/srgb.c"
#include "graphics/sqrgb.c"
#include "graphics/yuv.c"
#include "graphics/colour.c"
#include "graphics/blending.c"
#include "graphics/flattop_interpolation.c"
#include "graphics/blit.c"
#include "graphics/drawline.c"
#include "graphics/drawrect.c"
#include "graphics/drawpolygon.c"
#include "graphics/drawprimitives.c"
#ifndef __wasm__
#include "graphics/drawqueue.c"
#include "graphics/drawqueue_enqueue.c"
#include "graphics/drawqueue/drawqueue_soft.c"
#include "graphics/drawqueue/drawrect.c"
#include "graphics/drawqueue/drawline.c"
#include "graphics/drawqueue/blit.c"
#include "graphics/draw_effects.c"
#endif	// __wasm__
#include "graphics/processing.c"
#include "graphics/mipmap.c"
#include "graphics/cl_memory.c"
#include "graphics/texture_compression.c"
#include "vector/vector.c"
#include "vector/polyline.c"

#include "text/unicode_data.c"
#include "text/unicode.c"
#include "text/unicode_bidi.c"
#include "text/unicode_arabic.c"
#include "text/parse.c"
#include "text/format.c"
#include "text/string.c"
#include "text/edit.c"
#include "text/undo.c"
#include "text/history.c"
#include "vector_type/vector_type.c"
#include "vector_type/make_font.c"
#include "vector_type/draw.c"
#include "vector_type/stats.c"
#include "vector_type/fit.c"
#include "vector_type/cjk.c"
#include "vector_type/truetype.c"
#include "vector_type/insert_rect.c"

#include "gui/zoom.c"
#include "gui/focus.c"
#include "gui/positioning.c"
#include "gui/layout.c"
#include "gui/drawelements.c"
#include "gui/visualisations.c"
#include "gui/inputprocessing.c"
#include "gui/knob_functions.c"
#include "gui/controls.c"
#include "gui/control_array.c"
#include "gui/selection.c"
#include "gui/make_gui.c"
#include "gui/editor_toolbar.c"
#include "gui/floating_window.c"
#include "gui/window_manager.c"

#include "fileio/bits.c"
#include "fileio/open.c"
#include "fileio/io_override.c"
#include "fileio/endian.c"
#include "fileio/image.c"
#include "fileio/image_bmp.c"
#include "fileio/write_icc.c"
#include "fileio/image_tiff.c"
#include "fileio/image_tiff_lzw.c"
#include "fileio/image_fts.c"
#include "fileio/image_screen.c"
#include "fileio/sound_format.c"
#include "fileio/sound_aiff.c"
#include "fileio/sound_wav.c"
#include "fileio/path.c"
#include "fileio/dir.c"
#include "fileio/file_management.c"
#include "fileio/process.c"
#include "fileio/dynamic_loading.c"
#include "fileio/fileball.c"
#include "fileio/prefs.c"
#include "fileio/dialog.c"

#include "interpreter/interpreter.c"
#include "interpreter/compilation.c"
#include "interpreter/execution.c"
#include "interpreter/decompilation.c"
#include "interpreter/expression.c"

#include "wahe/wahe_module_side.c"

#include "libraries/opencl.c"
#include "libraries/sdl.c"
#include "libraries/vulkan.c"
#include "libraries/clfft.c"
#include "libraries/ffmpeg.c"
#include "libraries/ffmpeg_enc.c"
#include "fileio/ffmpeg_sound_player.c"
#include "libraries/opencv.c"
#include "libraries/devil.c"
#include "libraries/libsndfile.c"
#include "libraries/libraw.c"
#include "libraries/libjpeg.c"
#include "libraries/libstb_image.c"
#include "libraries/libstb_vorbis.c"
#include "libraries/libminimp3.c"
#include "libraries/libdr_flac.c"
#include "libraries/zlib.c"
#include "libraries/mpfr.c"
#include "libraries/fftpack.c"
#include "libraries/cfftpack.c"
#include "libraries/tinyexpr.c"
#include "libraries/libcurl.c"
#include "libraries/libminiaudio.c"
#include "libraries/openal.c"
#include "libraries/manymouse.c"
#include "libraries/emscripten.c"

#include "general/globals.c"
