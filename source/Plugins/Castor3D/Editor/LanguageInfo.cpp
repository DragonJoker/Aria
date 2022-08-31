#include "Editor/LanguageInfo.hpp"

#include "Editor/StyleInfo.hpp"

#include <AriaLib/Prerequisites.hpp>

namespace aria
{
	static wxColour const DefaultColour{ wxT( "#DCDCDC" ) };
	static wxColour const CommentColour{ wxT( "#608B4E" ) };
	static wxColour const StringColour{ wxT( "#D69D85" ) };
	static wxColour const LiteralColour{ wxT( "#B5CEA8" ) };
	static wxColour const Keyword1Colour{ wxT( "#569CD6" ) };
	static wxColour const Keyword2Colour{ wxT( "#FFBF80" ) };
	static wxColour const Keyword3Colour{ wxT( "#4EC9B0" ) };
	static wxColour const PreprocColour{ wxT( "SIENNA" ) };
	static wxColour const OperatorColour{ wxT( "#B4B4B4" ) };

	LanguageInfo::LanguageInfo()
		: name( "cscn" )
		, m_styles
		{
			{ wxSTC_C_DEFAULT, StyleInfo{ DefaultColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTLINE, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_NUMBER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_WORD, StyleInfo{ Keyword1Colour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_CHARACTER, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_UUID, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_PREPROCESSOR, StyleInfo{ PreprocColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_OPERATOR, StyleInfo{ OperatorColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_IDENTIFIER, StyleInfo{ Keyword3Colour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRINGEOL, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_VERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_REGEX, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTLINEDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_WORD2, StyleInfo{ Keyword2Colour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOCKEYWORD, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_COMMENTDOCKEYWORDERROR, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_GLOBALCLASS, StyleInfo{ DefaultColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_STRINGRAW, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_TRIPLEVERBATIM, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_HASHQUOTEDSTRING, StyleInfo{ StringColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
#if wxCHECK_VERSION( 3, 1, 0 )
			{ wxSTC_C_PREPROCESSORCOMMENT, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_PREPROCESSORCOMMENTDOC, StyleInfo{ CommentColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_USERLITERAL, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_TASKMARKER, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
			{ wxSTC_C_ESCAPESEQUENCE, StyleInfo{ LiteralColour, PANEL_BACKGROUND_COLOUR, 0, 0 } },
#endif
		}
	{
		m_keywords[0] = "animated_object animated_object_group animation billboard border_panel_overlay camera camera_node constants_buffer domain_program font geometry_program hull_program compute_program light material mesh object panel_overlay pass pixel_program positions render_target sampler scene scene_node shader_program skybox submesh technique texture_unit text_overlay variable vertex_program viewport window particle_system particle tf_shader_program cs_shader_program gui button static listbox combobox edit ssao subsurface_scattering smaa transmittance_profile hdr_config shadows linear_motion_blur elevation simplex_island rsm_config lpv_config raw_config pcf_config vsm_config voxel_cone_tracing default_materials draw_edges loading_screen render_pass water_rendering ocean_rendering waves wave theme static_style button_style slider_style combobox_style listbox_style edit_style line_style tick_style item_style selected_item_style highlighted_item_style pbrmr_texture_remap_config pbrsg_texture_remap_config phong_texture_remap_config skeleton morph_animation pbr_bloom atmospheric_scattering minRayleighDensity maxRayleighDensity minMieDensity maxMieDensity minAbsorptionDensity maxAbsorptionDensity fft_ocean_rendering fft_config diamond_square_terrain biome weather clouds";
		m_keywords[1] = "alpha alpha_blend alpha_blend_mode alpha_func ambient ambient_light aspect_ratio attenuation back background_colour background_image blend_func border_colour border_inner_uv border_material border_outer_uv mborder_panel_overlay border_position border_size bottom cast_shadows center_uv channel colour colour_blend_mode count cut_off debug_overlays diffuse dimensions division emissive exponent face face_normals face_tangents face_uv face_uvw far file fog_density fog_type format fov_y front fullscreen height horizontal_align image import include input_type intensity left line_spacing_mode lod_bias looped mag_filter materials max_anisotropy max_lod min_filter min_lod mip_filter import_morph mtl_file near normal normals orientation output_type output_vtx_count parent pos position postfx primitive pxl_border_size pxl_position pxl_size rgb_blend right scale shaders shadow_producer shininess size specular start_animation stereo tangent text text_overlay text_wrapping texturing_mode tone_mapping top two_sided type u_wrap_mode uv uvw v_wrap_mode value vertex vertical_align vsync w_wrap_mode particles_count receive_shadows equirectangular reflection_mapping default_font pixel_position pixel_size background_material text_material highlighted_background_material highlighted_foreground_material highlighted_text_material pushed_background_material pushed_foreground_material pushed_text_material pixel_border_size caption selected_item_background_material selected_item_foreground_material highlighted_item_background_material item multiline refraction_ratio enabled radius bias samples_count albedo roughness metalness glossiness specular_pbr visible direction distance_based_transmittance transmittance_coefficients gaussian_width strength mode preset reprojection factor pause_animation exposure gamma kernel_size parallax_occlusion num_samples edge_sharpness blur_step_size blur_radius high_quality use_normals_buffer blur_high_quality cross levels_count group_sizes anisotropic_filtering shadow_filter comparison_func comparison_mode producer filter volumetric_steps volumetric_scattering edgeDetection disableDiagonalDetection disableCornerDetection predication vectorDivider samples fpsScale default_material directional_shadow_cascades bw_accumulation max_slope_offset min_offset variance_max variance_bias occlusion_mask albedo_mask diffuse_mask normal_mask opacity_mask metalness_mask specular_mask roughness_mask glossiness_mask shininess_mask emissive_mask height_mask transmittance_mask normal_factor height_factor normal_directx mixed_interpolation pcf_width width color_range moisture_levels ssgiWidth blurSize min_radius reflective sample_count max_radius refractions reflections bend_step_count bend_step_size start_at stop_at invert_y lpv_indirect_attenuation texel_area_modifier global_illumination lpv_grid_size transmission translate rotate scale num_cones max_distance ray_step_size voxel_size conservative_rasterization grid_size temporal_smoothing secondary_bounce blend_alpha_func smooth_band_width edge_width edge_normal_factor edge_depth_factor edge_object_factor edge_colour normalDepthWidth objectWidth tile dampeningFactor refractionRatio refractionDistortionFactor refractionHeightFactor refractionDistanceFactor depthSofteningDistance normalMapScrollSpeed normalMapScroll ssrSettings normals1 normals2 noise tessellationFactor foamHeightStart foamFadeDistance foamTiling foamAngleExponent foamBrightness foam steepness length amplitude speed max_image_size rescale prefix cullable foreground_material enablePredication enableReprojection disabled_background_material disabled_foreground_material disabled_text_material angle no_optimisations file_anim import_anim pitch yaw roll animated_mesh animated_skeleton animated_node texcoord_set import_morph_target target_weight emissive_mult inner_cut_off outer_cut_off blurRadius bloomStrength passes depth transmittanceResolution multiScatterResolution atmosphereVolumeResolution solarIrradiance sunAngularRadius sunIlluminance sunIlluminanceScale rayMarchMinSPP rayMarchMaxSPP absorptionExtinction maxSunZenithAngle rayleighScattering mieScattering miePhaseFunctionG mieExtinction bottomRadius topRadius groundAlbedo multipleScatteringFactor layerWidth expTerm expScale linearTerm constantTerm skyViewResolution node detail uvScale xzScale heightRange disableRandomSeed gradient lod0Distance blocksCount patchSize windVelocity windDirection normalMapFreqMod displacementDownsample heightMapSamples density ssrDepthMult ssrBackwardStepsCount ssrForwardStepsCount ssrStepSize untile heatOffset island range lowSteepness mediumSteepness highSteepness worleyResolution perlinWorleyResolution weatherResolution curlResolution frequency octaves coverage crispiness curliness absorption innerRadius outerRadius topColour bottomColour enablePowder topOffset define";
		m_keywords[2] = "zero one src_colour inv_src_colour dst_colour inv_dst_colour src_alpha inv_src_alpha dst_alpha inv_dst_alpha constant inv_constant src_alpha_sat src1_colour inv_src1_colour src1_alpha inv_src1_alpha 1d 2d 3d always less less_equal equal not_equal greater_equal greater never texture texture0 texture1 texture2 texture3 constant diffuse previous none first_arg add add_signed modulate interpolate subtract dot3_rgb dot3_rgba none first_arg add add_signed modulate interpolate substract colour ambient diffuse normal specular height opacity emissive smooth flat point spot directional sm_1 sm_2 sm_3 sm_4 sm_5 ortho perspective frustum nearest linear repeat mirrored_repeat clamp_to_border clamp_to_edge vertex hull domain geometry pixel compute int sampler uint float vec2i vec3i vec4i vec2f vec3f vec4f mat3x3f mat4x4f camera light object billboard none break break_words internal middle external none additive multiplicative interpolative a_buffer depth_peeling top center bottom left center right letter text own_height max_lines_height max_font_height linear exponential squared_exponential custom cone cylinder sphere cube torus plane icosahedron projection cylindrical spherical phong reflection refraction pbr glossiness minimal 0extended transmittance 1X T2X S2X 4X low medium high ultra float_opaque_black float_transparent_black int_transparent_black int_opaque_black float_opaque_white int_opaque_white raw pcf variance max ref_to_texture luma colour depth ambient_occlusion occlusion point_list line_list line_strip triangle_list triangle_strip triangle_fan line_list_adj line_strip_adj triangle_list_adj triangle_strip_adj patch_list mixed lpv lpv_geometry layered_lpv layered_lpv_geometry rsm vct rgba32 blinn_phong toon_phong toon_blinn_phong toon_pbr true false screen_size rgb a r g b undefined rg8 rgba16 rgba16s rgb565 bgr565 rgba5551 bgra5551 argb1555 r8 r8s r8us r8ss r8ui r8srgb rg16 rg16s rg16us rg16ss rg16ui rg16si rg16srgb rgb24 rgb24s rgb24us rgb24ss rgb24ui rgb24si rgb24srgb bgr24 bgr24s bgr24us bgr24ss bgr24ui bgr24si bgr24srgb rgba32 rgba32s rgba32us rgba32ss rgba32ui rgba32si rgba32srgb bgra32 bgra32s bgra32us bgra32ss bgra32ui bgra32si bgra32srgb abgr32 abgr32s abgr32us abgr32ss abgr32ui abgr32si abgr32_stgb argb2101010 argb2101010s argb2101010us argb2101010ss argb2101010ui argb2101010si abgr2101010 abgr2101010s abgr2101010us abgr2101010ss abgr2101010ui abgr2101010si r16 rg16s rg16us rg16ss rg16ui rg16si rg16f rg32 rg32s rg32us rg32ss rg32ui rg32si rg32f rgb48 rgb48s rgb48us rgb48ss rgb48ui rgb48si rgb48f rgba64 rgba64s rgba64us rgba64ss rgba64ui rgba64si rgba64f r32ui r32si r32f rg64ui rg64si rg64f rgb96ui rgb96si rgb96f rgba128ui rgba128si rgba128f r64ui r64si r64f rg128ui rg128si rg128f rgb192ui rgb192si rgb192f rgba256ui rgba256si rgba256f bgr32f ebgr32f depth16 depth24 depth32f stencil8 depth16s8 depth24s8 depth32fs8 bc1_rgb bc1_srgb bc1_rgba bc1_rgba_srgb bc2_rgba bc2_rgba_srgb bc3_rgba bc3_rgba_srgb bc4_r bc4_r_s bc5_rg bc5_rg_s bc6h bc6h_s bc7 bc7_srgb etc2_rgb etc2_rgb_srgb etc2_rgba1 etc2_rgba1_srgb etc2_rgba etc2_rgba_srgb eac_r eac_r_s eac_rg eac_rg_s astc_4x4 astc_4x4_srgb astc_5x4 astc_5x4_srgb astc_5x5 astc_5x5_srgb astc_6x5 astc_6x5_srgb astc_6x6 astc_6x6_srgb astc_8x5 astc_8x5_srgb astc_8x6 astc_8x6_srgb astc_8x8 astc_8x8_srgb astc_10x5 astc_10x5_srgb astc_10x6 astc_10x6_srgb astc_10x8 astc_10x8_srgb astc_10x10 astc_10x10_srgb astc_12x10 astc_12x10_srgb astc_12x12 astc_12x12_srgb argb32";
	}
}
