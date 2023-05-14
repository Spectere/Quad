cmake_minimum_required(VERSION 3.25)
set(RENDER_SOFT Quad)
set(RENDER_GL QuadGL)

project(Quad C)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 99)

# Platform-specific definitions.
if(UNIX OR MINGW OR MSYS)
    add_compile_definitions(POSIX)
endif()

set(SRC_COMMON
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
    src/sbar.c                          src/sbar.h
                                        src/server.h
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

    # Renderer - Common
                                        src/render_common/common_anorms.h
                                        src/render_common/common_r_misc.h
                                        src/render_common/common_d_iface.h
                                        src/render_common/common_draw.h
                                        src/render_common/common_r_efrag.h
    src/render_common/common_r_part.c   src/render_common/common_r_part.h
                                        src/render_common/common_render.h
                                        src/render_common/common_screen.h

    # Sound
                                        src/sound/sound.h
    src/sound/snd_dma.c
    src/sound/snd_mem.c
    src/sound/snd_mix.c
    src/sound/snd_sdl2.c
)

set(SRC_RENDER_SOFT
                                        src/render_soft/soft_adivtab.h
    src/render_soft/soft_d_edge.c       src/render_soft/soft_d_edge.h
    src/render_soft/soft_d_init.c
                                        src/render_soft/soft_d_local.h
    src/render_soft/soft_d_modech.c
    src/render_soft/soft_d_part.c
    src/render_soft/soft_d_polyse.c
    src/render_soft/soft_d_scan.c       src/render_soft/soft_d_scan.h
    src/render_soft/soft_d_sky.c        src/render_soft/soft_d_sky.h
    src/render_soft/soft_d_sprite.c     src/render_soft/soft_d_sprite.h
    src/render_soft/soft_d_surf.c       src/render_soft/soft_d_surf.h
    src/render_soft/soft_d_vars.c
    src/render_soft/soft_draw.c
    src/render_soft/soft_model.c        src/render_soft/soft_model.h
    src/render_soft/soft_r_aclip.c      src/render_soft/soft_r_aclip.h
    src/render_soft/soft_r_alias.c      src/render_soft/soft_r_alias.h
    src/render_soft/soft_r_bsp.c        src/render_soft/soft_r_bsp.h
    src/render_soft/soft_r_draw.c       src/render_soft/soft_r_draw.h
    src/render_soft/soft_r_edge.c       src/render_soft/soft_r_edge.h
    src/render_soft/soft_r_efrag.c      src/render_soft/soft_r_efrag.h
    src/render_soft/soft_r_light.c      src/render_soft/soft_r_light.h
                                        src/render_soft/soft_r_local.h
    src/render_soft/soft_r_main.c       src/rendeR_soft/soft_r_main.h
    src/render_soft/soft_r_misc.c       src/render_soft/soft_r_misc.h
                                        src/render_soft/soft_r_shared.h
    src/render_soft/soft_r_sky.c        src/render_soft/soft_r_sky.h
    src/render_soft/soft_r_sprite.c     src/render_soft/soft_r_sprite.h
    src/render_soft/soft_r_surf.c       src/render_soft/soft_r_surf.h
    src/render_soft/soft_r_vars.c
    src/render_soft/soft_screen.c
)

set(SRC_RENDER_GL
                                        src/render_gl/gl_anorm_dots.h
    src/render_gl/gl_draw.c             src/render_gl/gl_draw.h
    src/render_gl/gl_mesh.c             src/render_gl/gl_mesh.h
    src/render_gl/gl_model.c            src/render_gl/gl_model.h
    src/render_gl/gl_r_efrag.c
    src/render_gl/gl_r_light.c          src/render_gl/gl_r_light.h
    src/render_gl/gl_r_main.c           src/render_gl/gl_r_main.h
    src/render_gl/gl_r_misc.c           src/render_gl/gl_r_misc.h
    src/render_gl/gl_r_surf.c           src/render_gl/gl_r_surf.h
    src/render_gl/gl_screen.c
    src/render_gl/gl_warp.c             src/render_gl/gl_warp.h
                                        src/render_gl/gl_warp_sin.h
                                        src/render_gl/glquake.h
)

# Common includes/libraries.
include_directories(${SDL2_INCLUDE_DIRS})
link_libraries(${SDL2_LIBRARIES})

# Common compiler settings.
add_compile_options(-Wall -Werror)

###                     ################################################################################################
###  SOFTWARE RENDERER  ################################################################################################
###                     ################################################################################################
add_executable(${RENDER_SOFT}
        ${SRC_COMMON}
        ${SRC_RENDER_SOFT}
)

target_compile_definitions(${RENDER_SOFT} PUBLIC
    -DRENDER_SOFT
)

##                   ###################################################################################################
##  OPENGL RENDERER  ###################################################################################################
##                   ###################################################################################################
add_executable(${RENDER_GL}
        ${SRC_COMMON}
        ${SRC_RENDER_GL}
)

target_compile_definitions(${RENDER_GL} PUBLIC
        -DRENDER_GL
)

target_link_libraries(${RENDER_GL}
        OpenGL::GL
)
