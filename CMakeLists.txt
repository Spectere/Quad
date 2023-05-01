cmake_minimum_required(VERSION 3.25)
set(RENDER_SOFT Quad)
set(RENDER_GL QuadGL)

project(Quad C)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 99)

set(SRC_COMMON
                                        src/render_common/anorms.h
                                        src/bspfile.h
    src/sound/cd_null.c
                                        src/sound/cdaudio.h
    src/chase.c
                                        src/client.h
    src/cl_demo.c
    src/cl_input.c
    src/cl_main.c
    src/cl_parse.c
    src/cl_tent.c
    src/cmd.c                           src/cmd.h
    src/common.c                        src/common.h
    src/console.c                       src/console.h
    src/crc.c                           src/crc.h
    src/cvar.c                          src/cvar.h
                                        src/render_common/d_iface.h
                                        src/render_common/d_local.h
                                        src/render_common/draw.h
    src/host.c
    src/host_cmd.c
                                        src/input.h
    src/keys.c                          src/keys.h
    src/mathlib.c                       src/mathlib.h
    src/menu.c                          src/menu.h
                                        src/modelgen.h
                                        src/net.h
    src/net_loop.c                      src/net_loop.h
    src/net_main.c
    src/net_none.c
    src/net_vcr.c                       src/net_vcr.h
    src/pr_cmds.c
                                        src/pr_comp.h
    src/pr_edict.c
    src/pr_exec.c
                                        src/progdefs.h
                                        src/progdefs.q1
                                        src/progs.h
                                        src/protocol.h
                                        src/quakedef.h
    src/render_common/r_part.c
                                        src/render_common/render.h
    src/sbar.c                          src/sbar.h
                                        src/render_common/screen.h
                                        src/server.h
    src/sound/snd_dma.c
    src/sound/snd_mem.c
    src/sound/snd_mix.c
    src/sound/snd_sdl2.c
                                        src/sound/sound.h
                                        src/spritegn.h
    src/sv_main.c
    src/sv_move.c
    src/sv_phys.c
    src/sv_user.c
                                        src/sys.h
    src/sys_sdl2.c                      src/sys_sdl2.h
                                        src/vid.h
    src/vid_sdl2.c
    src/view.c                          src/view.h
    src/wad.c                           src/wad.h
    src/world.c                         src/world.h
    src/zone.c                          src/zone.h
)

set(SRC_RENDER_SOFT
                                        src/render_soft/soft_adivtab.h
    src/render_soft/soft_d_edge.c
    src/render_soft/soft_d_fill.c
    src/render_soft/soft_d_init.c
    src/render_soft/soft_d_modech.c
    src/render_soft/soft_d_part.c
    src/render_soft/soft_d_polyse.c
    src/render_soft/soft_d_scan.c
    src/render_soft/soft_d_sky.c
    src/render_soft/soft_d_sprite.c
    src/render_soft/soft_d_surf.c
    src/render_soft/soft_d_vars.c
    src/render_soft/soft_d_zpoint.c
    src/render_soft/soft_draw.c
    src/render_soft/soft_model.c        src/render_soft/soft_model.h
    src/render_soft/soft_r_aclip.c
    src/render_soft/soft_r_alias.c
    src/render_soft/soft_r_bsp.c
    src/render_soft/soft_r_draw.c
    src/render_soft/soft_r_edge.c
    src/render_soft/soft_r_efrag.c
    src/render_soft/soft_r_light.c
                                        src/render_soft/soft_r_local.h
    src/render_soft/soft_r_main.c
    src/render_soft/soft_r_misc.c
                                        src/render_soft/soft_r_shared.h
    src/render_soft/soft_r_sky.c
    src/render_soft/soft_r_sprite.c
    src/render_soft/soft_r_surf.c
    src/render_soft/soft_r_vars.c
    src/render_soft/soft_screen.c
)

set(SRC_RENDER_GL
                                        src/render_gl/gl_anorm_dots.h
    src/render_gl/gl_draw.c
    src/render_gl/gl_mesh.c
    src/render_gl/gl_model.c            src/render_gl/gl_model.h
    src/render_gl/gl_refrag.c
    src/render_gl/gl_rlight.c
    src/render_gl/gl_rmain.c
    src/render_gl/gl_rmisc.c
    src/render_gl/gl_rsurf.c
    src/render_gl/gl_screen.c
    src/render_gl/gl_test.c
    src/render_gl/gl_warp.c             src/render_gl/gl_warp_sin.h
                                        src/render_gl/glquake.h
)

# Common includes/libraries.
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

# Common compiler settings.
add_compile_options(-Werror-implicit-function-declaration)

###                     ################################################################################################
###  SOFTWARE RENDERER  ################################################################################################
###                     ################################################################################################
add_executable(${RENDER_SOFT}
        ${SRC_COMMON}
        ${SRC_RENDER_SOFT}
)

##                   ###################################################################################################
##  OPENGL RENDERER  ###################################################################################################
##                   ###################################################################################################
add_executable(${RENDER_GL}
        ${SRC_COMMON}
        ${SRC_RENDER_GL}
)

target_compile_definitions(${RENDER_GL} PUBLIC
        -DGLQUAKE
)

target_link_libraries(${RENDER_GL}
        OpenGL::GL
)
