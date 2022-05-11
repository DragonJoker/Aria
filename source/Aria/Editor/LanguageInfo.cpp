#include "Editor/LanguageInfo.hpp"

#include "Prerequisites.hpp"
#include "Editor/StyleInfo.hpp"

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
		m_keywords[0] = "animated_object animated_object_group animation billboard border_panel_overlay camera camera_node constants_buffer domain_program font geometry_program hull_program compute_program light material mesh object panel_overlay pass pixel_program positions render_target sampler scene scene_node shader_program skybox submesh technique texture_unit text_overlay variable vertex_program viewport window particle_system particle tf_shader_program cs_shader_program gui button static listbox combobox edit ssao subsurface_scattering smaa transmittance_profile hdr_config shadows linear_motion_blur elevation simplex_island rsm_config lpv_config raw_config pcf_config vsm_config voxel_cone_tracing default_materials draw_edges loading_screen render_pass water_rendering ocean_rendering waves wave theme static_style button_style slider_style combobox_style listbox_style edit_style line_style tick_style item_style selected_item_style highlighted_item_style pbrmr_texture_remap_config pbrsg_texture_remap_config phong_texture_remap_config";
		m_keywords[1] = "alpha alpha_blend alpha_blend_mode alpha_func ambient ambient_light aspect_ratio attenuation back background_colour background_image blend_func border_colour border_inner_uv border_material border_outer_uv mborder_panel_overlay border_position border_size bottom cast_shadows center_uv channel colour colour_blend_mode count cut_off debug_overlays diffuse dimensions division emissive exponent face face_normals face_tangents face_uv face_uvw far file fog_density fog_type format fov_y front fullscreen height horizontal_align image import include input_type intensity left line_spacing_mode lod_bias looped mag_filter materials max_anisotropy max_lod min_filter min_lod mip_filter morph_import mtl_file near normal normals orientation output_type output_vtx_count parent pos position postfx primitive pxl_border_size pxl_position pxl_size rgb_blend right scale shaders shadow_producer shininess size specular start_animation stereo tangent text text_overlay text_wrapping texturing_mode tone_mapping top two_sided type u_wrap_mode uv uvw v_wrap_mode value vertex vertical_align vsync w_wrap_mode particles_count receive_shadows equirectangular reflection_mapping default_font pixel_position pixel_size background_material text_material highlighted_background_material highlighted_foreground_material highlighted_text_material pushed_background_material pushed_foreground_material pushed_text_material pixel_border_size caption selected_item_background_material selected_item_foreground_material highlighted_item_background_material item multiline refraction_ratio enabled radius bias samples_count albedo roughness metallic glossiness specular_pbr visible direction distance_based_transmittance transmittance_coefficients gaussian_width strength mode preset reprojection factor pause_animation exposure gamma kernel_size parallax_occlusion num_samples edge_sharpness blur_step_size blur_radius high_quality use_normals_buffer blur_high_quality cross levels_count group_sizes anisotropic_filtering shadow_filter comparison_func comparison_mode producer filter volumetric_steps volumetric_scattering edgeDetection disableDiagonalDetection disableCornerDetection predication vectorDivider samples fpsScale default_material directional_shadow_cascades bw_accumulation max_slope_offset min_offset variance_max variance_bias occlusion_mask albedo_mask diffuse_mask normal_mask opacity_mask metalness_mask specular_mask roughness_mask glossiness_mask shininess_mask emissive_mask height_mask transmittance_mask normal_factor height_factor normal_directx mixed_interpolation pcf_width width color_range moisture_levels ssgiWidth blurSize min_radius reflective sample_count max_radius refractions reflections bend_step_count bend_step_size start_at stop_at invert_y lpv_indirect_attenuation texel_area_modifier global_illumination lpv_grid_size transmission translate rotate scale num_cones max_distance ray_step_size voxel_size conservative_rasterization grid_size temporal_smoothing secondary_bounce blend_alpha_func smooth_band_width edge_width edge_normal_factor edge_depth_factor edge_object_factor edge_colour normalDepthWidth objectWidth tile dampeningFactor refractionRatio refractionDistortionFactor refractionHeightFactor refractionDistanceFactor depthSofteningDistance normalMapScrollSpeed normalMapScroll ssrSettings normals1 normals2 noise tessellationFactor foamHeightStart foamFadeDistance foamTiling foamAngleExponent foamBrightness foam steepness length amplitude speed max_image_size rescale prefix cullable foreground_material enablePredication enableReprojection disabled_background_material disabled_foreground_material disabled_text_material angle no_optimisations anim_file anim_import pitch yaw roll skeleton animated_mesh animated_skeleton animated_node define";
		m_keywords[2] = "zero one src_colour inv_src_colour dst_colour inv_dst_colour src_alpha inv_src_alpha dst_alpha inv_dst_alpha constant inv_constant src_alpha_sat src1_colour inv_src1_colour src1_alpha inv_src1_alpha 1d 2d 3d always less less_equal equal not_equal greater_equal greater never texture texture0 texture1 texture2 texture3 constant diffuse previous none first_arg add add_signed modulate interpolate subtract dot3_rgb dot3_rgba none first_arg add add_signed modulate interpolate substract colour ambient diffuse normal specular height opacity emissive smooth flat point spot directional sm_1 sm_2 sm_3 sm_4 sm_5 ortho perspective frustum nearest linear repeat mirrored_repeat clamp_to_border clamp_to_edge vertex hull domain geometry pixel compute int sampler uint float vec2i vec3i vec4i vec2f vec3f vec4f mat3x3f mat4x4f camera light object billboard none break break_words internal middle external none additive multiplicative interpolative a_buffer depth_peeling top center bottom left center right letter text own_height max_lines_height max_font_height linear exponential squared_exponential custom cone cylinder sphere cube torus plane icosahedron projection cylindrical spherical phong reflection refraction metallic_roughness specular_glossiness glossiness minimal 0extended transmittance 1X T2X S2X 4X low medium high ultra float_opaque_black float_transparent_black int_transparent_black int_opaque_black float_opaque_white int_opaque_white raw pcf variance max ref_to_texture luma colour depth ambient_occlusion occlusion point_list line_list line_strip triangle_list triangle_strip triangle_fan line_list_adj line_strip_adj triangle_list_adj triangle_strip_adj patch_list mixed lpv lpv_geometry layered_lpv layered_lpv_geometry rsm vct rgba32 blinn_phong toon_phong toon_blinn_phong toon_metallic_roughness toon_specular_glossiness true false screen_size l8 l16f l32f al16 al32f al16f argb1555 rgb565 argb16 rgb24 bgr24 argb32 abgr32 rgb16f argb16f rgb16f32f argb16f32f rgb32f argb32f dxtc1 dxtc3 dxtc5 yuy2 depth16 depth24 depth24s8 depth32 depth32f stencil1 stencil8 rgb a r g b";
	}
}
