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
		m_keywords[0] = "animated_object_group animation atmospheric_scattering"
			" billboard biome border_panel_overlay button button_style"
			" camera clouds combobox combobox_style constants_buffer"
			" default_materials density diamond_square_terrain draw_edges"
			" edit edit_style elements_style"
			" fft_config fft_ocean_rendering font"
			" gui"
			" highlighted_item_style"
			" hdr_config"
			" import item_style"
			" light listbox listbox_style lpv_config"
			" material materials mesh morph_animation motion_blur"
			" object ocean_rendering"
			" panel_overlay panel_style particle particle_system pass pbr_bloom pcf_config positions"
			" raw_config render_target rsm_config"
			" sampler scene scene_node selected_item_style shader_object shader_program shadows skeleton skybox slider slider_style smaa ssao static static_style style submesh subsurface_scattering"
			" text_overlay texture_animation texture_remap texture_remap_channel texture_transform texture_unit theme transmittance_profile"
			" variable viewport voxel_cone_tracing vsm_config"
			" water_rendering wave waves weather window"
			// Layout
			" layout_ctrl"
			// Box Layout
			" box_layout"
			// Panel control
			" panel panel_style"
			// Expandable Panel control
			" expandable_panel header content expandable_panel_style header_style expand_style content_style"
			// Frame control
			" frame frame_style";
		m_keywords[1] = "define include"
			" absorption absorptionExtinction albedo albedo_mask alpha alpha_blend_mode alpha_func ambient ambient_colour ambient_factor ambient_light amplitude animated_mesh animated_node animated_object animated_object_group animated_skeleton animation anisotropic_filtering aspect_ratio atmosphereVolumeResolution atmospheric_scattering attenuation attenuation_colour attenuation_distance"
			" back background_colour background_image background_invisible background_material bend_step_count bend_step_size bias billboard biome blend_alpha_func blocksCount bloomStrength blurRadius blur_high_quality blur_radius blur_step_size border_colour border_inner_uv border_material border_outer_uv border_panel_overlay border_position border_size bottom bottomColour bottomRadius button button_style bw_accumulation"
			" camera camera_node caption cast_shadows center_uv channel clearcoat clearcoat_factor clearcoat_mask clearcoat_normal clearcoat_normal_mask clearcoat_roughness clearcoat_roughness_factor clearcoat_roughness_mask clouds colour colour_blend_mode colour_hdr colour_mask colour_srgb combobox combobox_style comparison_func comparison_mode compute_program conservative_rasterization constantTerm constants_buffer cornerRounding count coverage crispiness cross cs_shader_program cullable curlResolution curliness cut_off"
			" dampeningFactor debug_overlays default_font default_material default_materials default_unit density depthSofteningDistance detail diamond_square_terrain diffuse diffuse_mask dimensions direction directional_shadow_cascades disableCornerDetection disableDiagonalDetection disableRandomSeed disabled_background_material disabled_foreground_material disabled_text_material displacementDownsample domain_program draw_edges"
			" edgeDetection edge_colour edge_depth_factor edge_normal_factor edge_object_factor edge_sharpness edge_width edit edit_style emissive emissive_colour emissive_factor emissive_mask emissive_mult enablePowder enablePredication enableReprojection enabled equirectangular expand expScale expTerm exponent exposure"
			" face face_normals face_tangents face_uv face_uvw factor far fft_config fft_ocean_rendering file file_anim filter filter_size foam foamAngleExponent foamBrightness foamFadeDistance foamHeightStart foamTiling fog_density fog_type font foreground_invisible foreground_material format fov_y fpsScale fractal frequency front fullscreen"
			" gamma gaussian_width geometry_program global_illumination glossiness glossiness_mask grid_size groundAlbedo group_sizes gui"
			" hdr_config heatOffset height heightMapSamples heightRange height_factor height_mask highSteepness high_quality highlighted_background_material highlighted_foreground_material highlighted_item_style highlighted_text_material horizontal_align hull_program"
			" image import import_anim import_morph_target innerRadius inner_cut_off intensity interpolation invert_y iridescence iridescence_factor iridescence_ior iridescence_mask iridescence_max_thickness iridescence_min_thickness iridescence_thickness iridescence_thickness_mask island item item_style"
			" layerWidth left length levels_count light light_bleeding_reduction lighting lighting_model line_spacing_mode line_style linearTerm linear_motion_blur listbox listbox_style loading_screen localContrastAdaptationFactor lod0Distance lod_bias looped lowSteepness lpv_config lpv_grid_size lpv_indirect_attenuation"
			" mag_filter material materials maxAbsorptionDensity maxMieDensity maxRayleighDensity maxSearchSteps maxSearchStepsDiag maxSunZenithAngle max_anisotropy max_distance max_image_size max_lod max_radius max_slope_offset mediumSteepness mesh metalness metalness_mask mieExtinction miePhaseFunctionG mieScattering minAbsorptionDensity minMieDensity minRayleighDensity min_filter min_lod min_offset min_radius min_variance mip_filter mixed_interpolation mode morph_animation movable multiScatterResolution multiline multipleScatteringFactor"
			" near no_optimisations noise normal normalDepthWidth normalMapFreqMod normalMapScroll normalMapScrollSpeed normal_directx normal_factor normal_mask normals1 normals2 num_cones num_samples"
			" object objectWidth occlusion occlusion_mask ocean_rendering octaves opacity opacity_mask orientation outerRadius outer_cut_off"
			" panel_overlay parallax_occlusion parent particle particle_system particles_count pass passes patchSize pause_animation pbr_bloom pcf_config perlinWorleyResolution pickable pitch pixel_border_size pixel_position pixel_program pixel_size planetNode pos position positions postfx predicationScale predicationStrength predicationThreshold prefix preset primitive producer pushed_background_material pushed_foreground_material pushed_text_material pxl_border_size pxl_position pxl_size"
			" radius range raw_config rayMarchMaxSPP rayMarchMinSPP ray_step_size rayleighScattering receive_shadows recenter_camera reflections refractionDistanceFactor refractionDistortionFactor refractionHeightFactor refractionRatio refraction_ratio render_pass render_target reprojectionWeightScale rescale right roll rotate roughness roughness_mask rsm_config"
			" sample_count sampler samples scale scene scene_node secondary_bounce selected_item_style shader_program shaders shadow_producer shadows sheen sheen_colour sheen_mask sheen_roughness sheen_roughness_mask shininess shininess_mask size skeleton skyViewResolution skybox slider slider_style smaa smooth_band_width solarIrradiance specular specular_mask speed ssao ssrBackwardStepsCount ssrDepthMult ssrForwardStepsCount ssrStepSize start_animation start_at static static_style steepness stereo stop_at strength submesh subsurface_scattering sunAngularRadius sunIlluminance sunIlluminanceScale sunNode"
			" tangent target_weight temporal_smoothing tessellationFactor texcoord_set texel_area_modifier text text_material text_overlay text_wrapping texture_remap_config texture_unit texturing_mode theme thickness thickness_factor thickness_mask threshold tick_style tile tiles tileset tone_mapping top topColour topOffset topRadius transform translate transmission transmission_mask transmittance transmittanceResolution transmittance_mask transmittance_profile two_sided type"
			" u_wrap_mode untile use_normals_buffer uv uvScale uvw"
			" v_wrap_mode value variable vectorDivider vertex vertex_program vertical_align viewport visible volumetric_scattering volumetric_steps voxel_cone_tracing voxel_size vsm_config vsync"
			" w_wrap_mode water_rendering wave waves weather weatherResolution windDirection windVelocity window worleyResolution"
			" xzScale"
			" yaw"
			// Layout
			" reserve_if_hidden stretch"
			// BoxLayout
			" horizontal layout_dynspace layout_staspace"
			// Layout Items Padding
			" padding pad_left pad_right pad_top pad_bottom"
			// Expandable Panel Control
			"  expand_caption retract_caption"
			// Frame Control
			"  header_font header_text_material header_caption header_horizontal_align header_vertical_align";
		m_keywords[2] = "zero one src_colour inv_src_colour dst_colour inv_dst_colour src_alpha inv_src_alpha dst_alpha inv_dst_alpha constant inv_constant src_alpha_sat src1_colour inv_src1_colour src1_alpha inv_src1_alpha 1d 2d 3d always less less_equal equal not_equal greater_equal greater never texture texture0 texture1 texture2 texture3 constant diffuse previous none first_arg add add_signed modulate interpolate subtract dot3_rgb dot3_rgba none first_arg add add_signed modulate interpolate substract colour ambient diffuse normal specular height opacity emissive smooth flat point spot directional sm_1 sm_2 sm_3 sm_4 sm_5 ortho perspective frustum nearest linear repeat mirrored_repeat clamp_to_border clamp_to_edge vertex hull domain geometry pixel compute int sampler uint float vec2i vec3i vec4i vec2f vec3f vec4f mat3x3f mat4x4f camera light object billboard none break break_words internal middle external none additive multiplicative interpolative a_buffer depth_peeling top center bottom left center right letter text own_height max_lines_height max_font_height linear exponential squared_exponential custom cone cylinder sphere cube torus plane icosahedron projection cylindrical spherical phong reflection refraction pbr glossiness minimal 0extended transmittance 1X T2X S2X 4X low medium high ultra float_opaque_black float_transparent_black int_transparent_black int_opaque_black float_opaque_white int_opaque_white raw pcf variance max ref_to_texture luma colour depth ambient_occlusion occlusion point_list line_list line_strip triangle_list triangle_strip triangle_fan line_list_adj line_strip_adj triangle_list_adj triangle_strip_adj patch_list mixed lpv lpv_geometry layered_lpv layered_lpv_geometry rsm vct rgba32 blinn_phong toon_phong toon_blinn_phong toon_pbr opacity km m cm mm yd ft in true false screen_size rgb a r g b undefined rg8 rgba16 rgba16s rgb565 bgr565 rgba5551 bgra5551 argb1555 r8 r8s r8us r8ss r8ui r8srgb rg16 rg16s rg16us rg16ss rg16ui rg16si rg16srgb rgb24 rgb24s rgb24us rgb24ss rgb24ui rgb24si rgb24srgb bgr24 bgr24s bgr24us bgr24ss bgr24ui bgr24si bgr24srgb rgba32 rgba32s rgba32us rgba32ss rgba32ui rgba32si rgba32srgb bgra32 bgra32s bgra32us bgra32ss bgra32ui bgra32si bgra32srgb abgr32 abgr32s abgr32us abgr32ss abgr32ui abgr32si abgr32_stgb argb2101010 argb2101010s argb2101010us argb2101010ss argb2101010ui argb2101010si abgr2101010 abgr2101010s abgr2101010us abgr2101010ss abgr2101010ui abgr2101010si r16 rg16s rg16us rg16ss rg16ui rg16si rg16f rg32 rg32s rg32us rg32ss rg32ui rg32si rg32f rgb48 rgb48s rgb48us rgb48ss rgb48ui rgb48si rgb48f rgba64 rgba64s rgba64us rgba64ss rgba64ui rgba64si rgba64f r32ui r32si r32f rg64ui rg64si rg64f rgb96ui rgb96si rgb96f rgba128ui rgba128si rgba128f r64ui r64si r64f rg128ui rg128si rg128f rgb192ui rgb192si rgb192f rgba256ui rgba256si rgba256f bgr32f ebgr32f depth16 depth24 depth32f stencil8 depth16s8 depth24s8 depth32fs8 bc1_rgb bc1_srgb bc1_rgba bc1_rgba_srgb bc2_rgba bc2_rgba_srgb bc3_rgba bc3_rgba_srgb bc4_r bc4_r_s bc5_rg bc5_rg_s bc6h bc6h_s bc7 bc7_srgb etc2_rgb etc2_rgb_srgb etc2_rgba1 etc2_rgba1_srgb etc2_rgba etc2_rgba_srgb eac_r eac_r_s eac_rg eac_rg_s astc_4x4 astc_4x4_srgb astc_5x4 astc_5x4_srgb astc_5x5 astc_5x5_srgb astc_6x5 astc_6x5_srgb astc_6x6 astc_6x6_srgb astc_8x5 astc_8x5_srgb astc_8x6 astc_8x6_srgb astc_8x8 astc_8x8_srgb astc_10x5 astc_10x5_srgb astc_10x6 astc_10x6_srgb astc_10x8 astc_10x8_srgb astc_10x10 astc_10x10_srgb astc_12x10 astc_12x10_srgb astc_12x12 astc_12x12_srgb argb32";
	}
}
