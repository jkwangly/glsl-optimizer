/*
 * Copyright © 2013 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \file builtin_types.cpp
 *
 * The glsl_type class has static members to represent all the built-in types
 * (such as the glsl_type::_float_type flyweight) as well as convenience pointer
 * accessors (such as glsl_type::float_type).  Those global variables are
 * declared and initialized in this file.
 *
 * This also contains _mesa_glsl_initialize_types(), a function which populates
 * a symbol table with the available built-in types for a particular language
 * version and set of enabled extensions.
 */

#include "glsl_types.h"
#include "glsl_parser_extras.h"

/**
 * Declarations of type flyweights (glsl_type::_foo_type) and
 * convenience pointers (glsl_type::foo_type).
 * @{
 */
#define DECL_TYPE(NAME, ...)                                    \
   const glsl_type glsl_type::_##NAME##_type = glsl_type(__VA_ARGS__, #NAME); \
   const glsl_type *const glsl_type::NAME##_type = &glsl_type::_##NAME##_type;

#define STRUCT_TYPE(NAME)                                       \
   const glsl_type glsl_type::_struct_##NAME##_type =           \
      glsl_type(NAME##_fields, Elements(NAME##_fields), #NAME); \
   const glsl_type *const glsl_type::struct_##NAME##_type =     \
      &glsl_type::_struct_##NAME##_type;

static const struct glsl_struct_field gl_DepthRangeParameters_fields[] = {
   { glsl_type::float_type, "near", false },
   { glsl_type::float_type, "far",  false },
   { glsl_type::float_type, "diff", false },
};

static const struct glsl_struct_field gl_PointParameters_fields[] = {
   { glsl_type::float_type, "size", false },
   { glsl_type::float_type, "sizeMin", false },
   { glsl_type::float_type, "sizeMax", false },
   { glsl_type::float_type, "fadeThresholdSize", false },
   { glsl_type::float_type, "distanceConstantAttenuation", false },
   { glsl_type::float_type, "distanceLinearAttenuation", false },
   { glsl_type::float_type, "distanceQuadraticAttenuation", false },
};

static const struct glsl_struct_field gl_MaterialParameters_fields[] = {
   { glsl_type::vec4_type, "emission", false },
   { glsl_type::vec4_type, "ambient", false },
   { glsl_type::vec4_type, "diffuse", false },
   { glsl_type::vec4_type, "specular", false },
   { glsl_type::float_type, "shininess", false },
};

static const struct glsl_struct_field gl_LightSourceParameters_fields[] = {
   { glsl_type::vec4_type, "ambient", false },
   { glsl_type::vec4_type, "diffuse", false },
   { glsl_type::vec4_type, "specular", false },
   { glsl_type::vec4_type, "position", false },
   { glsl_type::vec4_type, "halfVector", false },
   { glsl_type::vec3_type, "spotDirection", false },
   { glsl_type::float_type, "spotExponent", false },
   { glsl_type::float_type, "spotCutoff", false },
   { glsl_type::float_type, "spotCosCutoff", false },
   { glsl_type::float_type, "constantAttenuation", false },
   { glsl_type::float_type, "linearAttenuation", false },
   { glsl_type::float_type, "quadraticAttenuation", false },
};

static const struct glsl_struct_field gl_LightModelParameters_fields[] = {
   { glsl_type::vec4_type, "ambient", false },
};

static const struct glsl_struct_field gl_LightModelProducts_fields[] = {
   { glsl_type::vec4_type, "sceneColor", false },
};

static const struct glsl_struct_field gl_LightProducts_fields[] = {
   { glsl_type::vec4_type, "ambient", false },
   { glsl_type::vec4_type, "diffuse", false },
   { glsl_type::vec4_type, "specular", false },
};

static const struct glsl_struct_field gl_FogParameters_fields[] = {
   { glsl_type::vec4_type, "color", false },
   { glsl_type::float_type, "density", false },
   { glsl_type::float_type, "start", false },
   { glsl_type::float_type, "end", false },
   { glsl_type::float_type, "scale", false },
};

#include "builtin_type_macros.h"
/** @} */

/**
 * Code to populate a symbol table with the built-in types available in a
 * particular shading language version.  The table below contains tags every
 * type with the GLSL/GLSL ES versions where it was introduced.
 *
 * @{
 */
#define T(TYPE, MIN_GL, MIN_ES) \
   { glsl_type::TYPE##_type, MIN_GL, MIN_ES },

const static struct builtin_type_versions {
   const glsl_type *const type;
   int min_gl;
   int min_es;
} builtin_type_versions[] = {
   T(void,                            110, 100)
   T(bool,                            110, 100)
   T(bvec2,                           110, 100)
   T(bvec3,                           110, 100)
   T(bvec4,                           110, 100)
   T(int,                             110, 100)
   T(ivec2,                           110, 100)
   T(ivec3,                           110, 100)
   T(ivec4,                           110, 100)
   T(uint,                            130, 300)
   T(uvec2,                           130, 300)
   T(uvec3,                           130, 300)
   T(uvec4,                           130, 300)
   T(float,                           110, 100)
   T(vec2,                            110, 100)
   T(vec3,                            110, 100)
   T(vec4,                            110, 100)
   T(mat2,                            110, 100)
   T(mat3,                            110, 100)
   T(mat4,                            110, 100)
   T(mat2x3,                          120, 300)
   T(mat2x4,                          120, 300)
   T(mat3x2,                          120, 300)
   T(mat3x4,                          120, 300)
   T(mat4x2,                          120, 300)
   T(mat4x3,                          120, 300)

   T(sampler1D,                       110, 999)
   T(sampler2D,                       110, 100)
   T(sampler3D,                       110, 300)
   T(samplerCube,                     110, 100)
   T(sampler1DArray,                  130, 999)
   T(sampler2DArray,                  130, 300)
   T(samplerCubeArray,                400, 999)
   T(sampler2DRect,                   140, 999)
   T(samplerBuffer,                   140, 999)
   T(sampler2DMS,                     150, 999)
   T(sampler2DMSArray,                150, 999)

   T(isampler1D,                      130, 999)
   T(isampler2D,                      130, 300)
   T(isampler3D,                      130, 300)
   T(isamplerCube,                    130, 300)
   T(isampler1DArray,                 130, 999)
   T(isampler2DArray,                 130, 300)
   T(isamplerCubeArray,               400, 999)
   T(isampler2DRect,                  140, 999)
   T(isamplerBuffer,                  140, 999)
   T(isampler2DMS,                    150, 999)
   T(isampler2DMSArray,               150, 999)

   T(usampler1D,                      130, 999)
   T(usampler2D,                      130, 300)
   T(usampler3D,                      130, 300)
   T(usamplerCube,                    130, 300)
   T(usampler1DArray,                 130, 999)
   T(usampler2DArray,                 130, 300)
   T(usamplerCubeArray,               400, 999)
   T(usampler2DRect,                  140, 999)
   T(usamplerBuffer,                  140, 999)
   T(usampler2DMS,                    150, 999)
   T(usampler2DMSArray,               150, 999)

   T(sampler1DShadow,                 110, 999)
   T(sampler2DShadow,                 110, 300)
   T(samplerCubeShadow,               130, 300)
   T(sampler1DArrayShadow,            130, 999)
   T(sampler2DArrayShadow,            130, 300)
   T(samplerCubeArrayShadow,          400, 999)
   T(sampler2DRectShadow,             140, 999)

   T(struct_gl_DepthRangeParameters,  110, 100)
};

const glsl_type *const deprecated_types[] = {
   glsl_type::struct_gl_PointParameters_type,
   glsl_type::struct_gl_MaterialParameters_type,
   glsl_type::struct_gl_LightSourceParameters_type,
   glsl_type::struct_gl_LightModelParameters_type,
   glsl_type::struct_gl_LightModelProducts_type,
   glsl_type::struct_gl_LightProducts_type,
   glsl_type::struct_gl_FogParameters_type,
};

static inline void
add_type(glsl_symbol_table *symbols, const glsl_type *const type)
{
   symbols->add_type(type->name, type);
}

/**
 * Populate the symbol table with available built-in types.
 */
void
_mesa_glsl_initialize_types(struct _mesa_glsl_parse_state *state)
{
   struct glsl_symbol_table *symbols = state->symbols;

   for (unsigned i = 0; i < Elements(builtin_type_versions); i++) {
      const struct builtin_type_versions *const t = &builtin_type_versions[i];
      if (state->is_version(t->min_gl, t->min_es)) {
         add_type(symbols, t->type);
      }
   }

   /* Add deprecated structure types.  While these were deprecated in 1.30,
    * they're still present.  We've removed them in 1.40+ (OpenGL 3.1+).
    */
   if (!state->es_shader && state->language_version < 140) {
      for (unsigned i = 0; i < Elements(deprecated_types); i++) {
         add_type(symbols, deprecated_types[i]);
      }
   }

   /* Add types for enabled extensions.  They may have already been added
    * by the version-based loop, but attempting to add them a second time
    * is harmless.
    */
   if (state->ARB_texture_cube_map_array_enable) {
      add_type(symbols, glsl_type::samplerCubeArray_type);
      add_type(symbols, glsl_type::samplerCubeArrayShadow_type);
      add_type(symbols, glsl_type::isamplerCubeArray_type);
      add_type(symbols, glsl_type::usamplerCubeArray_type);
   }

   if (state->ARB_texture_multisample_enable) {
      add_type(symbols, glsl_type::sampler2DMS_type);
      add_type(symbols, glsl_type::isampler2DMS_type);
      add_type(symbols, glsl_type::usampler2DMS_type);
      add_type(symbols, glsl_type::sampler2DMSArray_type);
      add_type(symbols, glsl_type::isampler2DMSArray_type);
      add_type(symbols, glsl_type::usampler2DMSArray_type);
   }

   if (state->ARB_texture_rectangle_enable) {
      add_type(symbols, glsl_type::sampler2DRect_type);
      add_type(symbols, glsl_type::sampler2DRectShadow_type);
   }

   if (state->EXT_texture_array_enable) {
      add_type(symbols, glsl_type::sampler1DArray_type);
      add_type(symbols, glsl_type::sampler2DArray_type);
      add_type(symbols, glsl_type::sampler1DArrayShadow_type);
      add_type(symbols, glsl_type::sampler2DArrayShadow_type);
   }

   if (state->OES_EGL_image_external_enable) {
      add_type(symbols, glsl_type::samplerExternalOES_type);
   }

   if (state->EXT_shadow_samplers_enable) {
      add_type(symbols, glsl_type::sampler2DShadow_type);
   }

   if (state->OES_texture_3D_enable) {
      add_type(symbols, glsl_type::sampler3D_type);
   }
}
/** @} */
