//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_constantSlider;
	Fl_Slider*			m_linearSlider;
	Fl_Slider*			m_quadricSlider;
	Fl_Slider*			m_ambientSlider;
	Fl_Slider*			m_thresholdSlider;
	Fl_Slider*			m_supPixelSlider;
	Fl_Slider*			m_focalLengthSlider;
	Fl_Slider*			m_apertureSizeSlider;

	Fl_Light_Button*	m_SuperSampButton;
	Fl_Light_Button*	m_AdaptSupperButton;
	Fl_Light_Button*	m_JitterButton;
	Fl_Light_Button*	m_backgroundButton;
	Fl_Light_Button*	m_textureButton;
	Fl_Light_Button*	m_bumpMappingButton;
	Fl_Light_Button*	m_DOFButton;
	Fl_Light_Button*	m_glossyButton;
	Fl_Light_Button*	m_softShadowButton;
	Fl_Light_Button*	m_motionButton;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	int			m_getSupPixel() { return m_nSupPixel; }

	float		getConstant() { return m_fConstant; }
	float		getLinear() { return m_fLinear; }
	float		getQuadric() { return m_fQuadric; }
	float		getAmbient() { return m_fAmbient; }
	float		getThreshhold() { return m_fThreshhold; }

	bool		getSuperSamp() { return m_bSuperSamp; }
	bool		getAdaptSupper() { return m_bAdaptSupper; }
	bool		getJitter() { return m_bJitter; }
	bool		getBackground() { return m_bBackground; }
	bool		getTexture() { return m_bTexture; }
	bool		getBumpMapping() { return m_bBumpMapping; }
	bool		getDOF() { return m_bDOF; }
	bool		getGlossy() { return m_bGlossy; }
	bool		getSoftShadow() { return m_bSoftShadow; }
	bool		getMotion() { return m_bMotion; }

	float		getFocalLength() { return m_fFocalLength; }
	int			getApertureSize() { return m_nApertureSize; }

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	int			m_nSupPixel;
	int			m_nApertureSize;

	float		m_fConstant;
	float		m_fLinear;
	float		m_fQuadric;
	float		m_fAmbient;
	float		m_fThreshhold;
	float		m_fFocalLength;

	bool		m_bSuperSamp;
	bool		m_bAdaptSupper;
	bool		m_bJitter;
	bool		m_bBackground;
	bool		m_bTexture;
	bool		m_bBumpMapping;
	bool		m_bDOF;
	bool		m_bGlossy;
	bool		m_bSoftShadow;
	bool		m_bMotion;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);

	static void cb_constantSlides(Fl_Widget* o, void* v);
	static void cb_linearSlides(Fl_Widget* o, void* v);
	static void cb_quadricSlides(Fl_Widget* o, void* v);
	static void cb_ambientSlides(Fl_Widget* o, void* v);
	static void cb_threshholdSlides(Fl_Widget* o, void* v);

	static void cb_superSampButton(Fl_Widget* o, void* v);
	static void cb_adaptSupperButton(Fl_Widget* o, void* v);
	static void cb_jitterButton(Fl_Widget* o, void* v);

	static void cb_supPixelSlides(Fl_Widget* o, void* v);

	static void cb_backgroundButton(Fl_Widget* o, void* v);
	static void cb_textureButton(Fl_Widget* o, void* v);
	static void cb_bumpMappingButton(Fl_Widget* o, void* v);

	static void cb_DOFButton(Fl_Widget* o, void* v);
	static void cb_glossyButton(Fl_Widget* o, void* v);
	static void cb_softShadowButton(Fl_Widget* o, void* v);
	static void cb_motionButton(Fl_Widget* o, void* v);

	static void cb_focalLengthSlider(Fl_Widget* o, void* v);
	static void cb_apertureSizeSlider(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
