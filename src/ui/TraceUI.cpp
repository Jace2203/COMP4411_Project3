//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"
#include "../fileio/bitmap.h"

static bool done;
extern RayTracer* theRayTracer;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_load_background(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.bmp", NULL );

	if (newfile != NULL) {
		int				width,
						height;

		if ( (pUI->raytracer->background=readBMP(newfile, width, height))==NULL )
		{
			fl_alert("Can't load bitmap file");
		}

		pUI->raytracer->bgwidth = width;
		pUI->raytracer->bgheight = height;
		pUI->raytracer->bgloaded = true;;
	}
}

void TraceUI::cb_load_hfield(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.bmp", NULL );

	if (newfile != NULL) {
		unsigned char*	data;
		int				width,
						height;

		if ( (data=readBMP(newfile, width, height))==NULL )
		{
			fl_alert("Can't load bitmap file");
		}

		char* pch = strtok (newfile, ".");
		unsigned char* grey;
		if ( (grey=readBMP(strcat(pch, "_grey.bmp"), width, height))==NULL )
		{
			fl_alert("Can't load bitmap file");
		}

		pUI->raytracer->loadHField(data, grey, width, height);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_constantSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fConstant=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_linearSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fLinear=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_quadricSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fQuadric=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_ambientSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fAmbient=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_threshholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fThreshhold=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_superSampButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bSuperSamp)
	{
		((TraceUI*)(o->user_data()))->m_bSuperSamp = false;
		((TraceUI*)(o->user_data()))->m_AdaptSupperButton->activate();
	}
	else
	{
		((TraceUI*)(o->user_data()))->m_bSuperSamp = true;
		((TraceUI*)(o->user_data()))->m_AdaptSupperButton->deactivate();
	}
}

void TraceUI::cb_adaptSupperButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bAdaptSupper)
	{
		((TraceUI*)(o->user_data()))->m_bAdaptSupper = false;
		((TraceUI*)(o->user_data()))->m_SuperSampButton->activate();
	}
	else
	{
		((TraceUI*)(o->user_data()))->m_bAdaptSupper = true;
		((TraceUI*)(o->user_data()))->m_SuperSampButton->deactivate();
	}
}

void TraceUI::cb_jitterButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bJitter)
		((TraceUI*)(o->user_data()))->m_bJitter = false;
	else ((TraceUI*)(o->user_data()))->m_bJitter = true;
}

void TraceUI::cb_supPixelSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSupPixel=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_backgroundButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bBackground)
		((TraceUI*)(o->user_data()))->m_bBackground = false;
	else ((TraceUI*)(o->user_data()))->m_bBackground = true;
}

void TraceUI::cb_textureButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bTexture)
		((TraceUI*)(o->user_data()))->m_bTexture = false;
	else ((TraceUI*)(o->user_data()))->m_bTexture = true;
}

void TraceUI::cb_bumpMappingButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bBumpMapping)
		((TraceUI*)(o->user_data()))->m_bBumpMapping = false;
	else ((TraceUI*)(o->user_data()))->m_bBumpMapping = true;
}

void TraceUI::cb_DOFButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bDOF)
		((TraceUI*)(o->user_data()))->m_bDOF = false;
	else ((TraceUI*)(o->user_data()))->m_bDOF = true;
}

void TraceUI::cb_glossyButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bGlossy)
		((TraceUI*)(o->user_data()))->m_bGlossy = false;
	else ((TraceUI*)(o->user_data()))->m_bGlossy = true;
}

void TraceUI::cb_softShadowButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bSoftShadow)
		((TraceUI*)(o->user_data()))->m_bSoftShadow = false;
	else ((TraceUI*)(o->user_data()))->m_bSoftShadow = true;
}

void TraceUI::cb_motionButton(Fl_Widget* o, void* v)
{
	if (((TraceUI*)(o->user_data()))->m_bMotion)
		((TraceUI*)(o->user_data()))->m_bMotion = false;
	else ((TraceUI*)(o->user_data()))->m_bMotion = true;
}

void TraceUI::cb_focalLengthSlider(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_fFocalLength=float( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_apertureSizeSlider(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nApertureSize=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_aabbButton(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bAABB = !((TraceUI*)(o->user_data()))->m_bAABB;
}

void TraceUI::cb_noiseInputSlider(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nNoiseInput=int( ((Fl_Slider *)o)->value() );
}

void TraceUI::cb_noiseScaleSlider(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nNoiseScale=int( ((Fl_Slider *)o)->value() );
}

void TraceUI::cb_noiseTextureButton(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_bNoiseTexture = !((TraceUI*)(o->user_data()))->m_bNoiseTexture;
}

void TraceUI::cb_generateNoiseButton(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_noiseTextureButton->activate();
	theRayTracer->setNoiseTexture();
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		srand(time(NULL));

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		printf("finish");
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Load Background Image",	FL_ALT + 'b', (Fl_Callback *)TraceUI::cb_load_background },
		{ "&Load HField Map",	FL_ALT + 'h', (Fl_Callback *)TraceUI::cb_load_hfield },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;

	m_fConstant = 0.25;
	m_fLinear = 0.25;
	m_fQuadric = 0.5;
	m_fAmbient = 0.2;
	m_fThreshhold = 0;

	m_bSuperSamp = false;
	m_bAdaptSupper = false;
	m_bJitter = false;

	m_nSupPixel = 2;

	m_bBackground = false;
	m_bTexture = false;
	m_bBumpMapping = false;

	m_bDOF = false;
	m_bGlossy = false;
	m_bSoftShadow = false;
	m_bMotion = false;

	m_nApertureSize = 2;
	m_fFocalLength = 2;

	m_bAABB = false;

	m_nNoiseInput = 3;
	m_nNoiseScale = 2;
	m_bNoiseTexture = false;

	m_mainWindow = new Fl_Window(100, 40, 350, 480, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 350, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		// install slider Constant
		m_constantSlider = new Fl_Value_Slider(10, 80, 180, 20, "Attenuation, Constant");
		m_constantSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_constantSlider->type(FL_HOR_NICE_SLIDER);
        m_constantSlider->labelfont(FL_COURIER);
        m_constantSlider->labelsize(12);
		m_constantSlider->minimum(0);
		m_constantSlider->maximum(1);
		m_constantSlider->step(0.01);
		m_constantSlider->value(m_fConstant);
		m_constantSlider->align(FL_ALIGN_RIGHT);
		m_constantSlider->callback(cb_constantSlides);

		// install slider Linear
		m_linearSlider = new Fl_Value_Slider(10, 105, 180, 20, "Attenuation, Linear");
		m_linearSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_linearSlider->type(FL_HOR_NICE_SLIDER);
        m_linearSlider->labelfont(FL_COURIER);
        m_linearSlider->labelsize(12);
		m_linearSlider->minimum(0);
		m_linearSlider->maximum(1);
		m_linearSlider->step(0.01);
		m_linearSlider->value(m_fLinear);
		m_linearSlider->align(FL_ALIGN_RIGHT);
		m_linearSlider->callback(cb_linearSlides);

		// install slider Quadric
		m_quadricSlider = new Fl_Value_Slider(10, 130, 180, 20, "Attenuation, Quadric");
		m_quadricSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_quadricSlider->type(FL_HOR_NICE_SLIDER);
        m_quadricSlider->labelfont(FL_COURIER);
        m_quadricSlider->labelsize(12);
		m_quadricSlider->minimum(0);
		m_quadricSlider->maximum(1);
		m_quadricSlider->step(0.01);
		m_quadricSlider->value(m_fQuadric);
		m_quadricSlider->align(FL_ALIGN_RIGHT);
		m_quadricSlider->callback(cb_quadricSlides);

		// install slider Ambient
		m_ambientSlider = new Fl_Value_Slider(10, 155, 180, 20, "Ambient Light");
		m_ambientSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ambientSlider->type(FL_HOR_NICE_SLIDER);
        m_ambientSlider->labelfont(FL_COURIER);
        m_ambientSlider->labelsize(12);
		m_ambientSlider->minimum(0);
		m_ambientSlider->maximum(1);
		m_ambientSlider->step(0.01);
		m_ambientSlider->value(m_fAmbient);
		m_ambientSlider->align(FL_ALIGN_RIGHT);
		m_ambientSlider->callback(cb_ambientSlides);

		// install slider Threshhold
		m_thresholdSlider = new Fl_Value_Slider(10, 180, 180, 20, "Threshhold");
		m_thresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_thresholdSlider->type(FL_HOR_NICE_SLIDER);
        m_thresholdSlider->labelfont(FL_COURIER);
        m_thresholdSlider->labelsize(12);
		m_thresholdSlider->minimum(0);
		m_thresholdSlider->maximum(1);
		m_thresholdSlider->step(0.01);
		m_thresholdSlider->value(m_fThreshhold);
		m_thresholdSlider->align(FL_ALIGN_RIGHT);
		m_thresholdSlider->callback(cb_threshholdSlides);

		// install lightbutton Super Samp
		m_SuperSampButton = new Fl_Light_Button(10, 205, 120, 25,"&Super Samp");
		m_SuperSampButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_SuperSampButton->callback(cb_superSampButton);

		// install lightbutton Adapt Supper
		m_AdaptSupperButton = new Fl_Light_Button(140, 205, 130, 25,"&Adapt Supper");
		m_AdaptSupperButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_AdaptSupperButton->callback(cb_adaptSupperButton);

		// install lightbutton Jitter
		m_JitterButton = new Fl_Light_Button(280, 205, 60, 25,"&Jitter");
		m_JitterButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_JitterButton->callback(cb_jitterButton);

		// install slider Sup-pixel
		m_supPixelSlider = new Fl_Value_Slider(10, 235, 180, 20, "Number of sup-pixel");
		m_supPixelSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_supPixelSlider->type(FL_HOR_NICE_SLIDER);
        m_supPixelSlider->labelfont(FL_COURIER);
        m_supPixelSlider->labelsize(12);
		m_supPixelSlider->minimum(1);
		m_supPixelSlider->maximum(5);
		m_supPixelSlider->step(1);
		m_supPixelSlider->value(m_nSupPixel);
		m_supPixelSlider->align(FL_ALIGN_RIGHT);
		m_supPixelSlider->callback(cb_supPixelSlides);

		// install lightbutton Background
		m_backgroundButton = new Fl_Light_Button(10, 260, 110, 25,"&Background");
		m_backgroundButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_backgroundButton->callback(cb_backgroundButton);
		m_backgroundButton->deactivate();

		// install lightbutton Texture
		m_textureButton = new Fl_Light_Button(135, 260, 85, 25,"&Texture");
		m_textureButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_textureButton->callback(cb_textureButton);
		m_textureButton->deactivate();

		// install lightbutton Bump mapping
		m_bumpMappingButton = new Fl_Light_Button(220, 260, 120, 25,"&Bump mapping");
		m_bumpMappingButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_bumpMappingButton->callback(cb_bumpMappingButton);
		// m_bumpMappingButton->deactivate();

		// install lightbutton DOP
		m_DOFButton = new Fl_Light_Button(10, 290, 60, 25,"&DOF");
		m_DOFButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_DOFButton->callback(cb_DOFButton);

		// install lightbutton Glossy
		m_glossyButton = new Fl_Light_Button(80, 290, 70, 25,"&Glossy");
		m_glossyButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_glossyButton->callback(cb_glossyButton);

		// install lightbutton Soft Shadow
		m_softShadowButton = new Fl_Light_Button(160, 290, 100, 25,"&Soft Shadow");
		m_softShadowButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_softShadowButton->callback(cb_softShadowButton);

		// install lightbutton Motion
		m_motionButton = new Fl_Light_Button(270, 290, 70, 25,"&Motion");
		m_motionButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_motionButton->callback(cb_motionButton);

		// install slider Focal Length
		m_focalLengthSlider = new Fl_Value_Slider(10, 320, 180, 20, "Focal Length");
		m_focalLengthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_focalLengthSlider->type(FL_HOR_NICE_SLIDER);
        m_focalLengthSlider->labelfont(FL_COURIER);
        m_focalLengthSlider->labelsize(12);
		m_focalLengthSlider->minimum(1);
		m_focalLengthSlider->maximum(5);
		m_focalLengthSlider->step(0.01);
		m_focalLengthSlider->value(m_fFocalLength);
		m_focalLengthSlider->align(FL_ALIGN_RIGHT);
		m_focalLengthSlider->callback(cb_focalLengthSlider);

		// install slider Aperture Size
		m_apertureSizeSlider = new Fl_Value_Slider(10, 345, 180, 20, "Aperture Size");
		m_apertureSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_apertureSizeSlider->type(FL_HOR_NICE_SLIDER);
        m_apertureSizeSlider->labelfont(FL_COURIER);
        m_apertureSizeSlider->labelsize(12);
		m_apertureSizeSlider->minimum(1);
		m_apertureSizeSlider->maximum(5);
		m_apertureSizeSlider->step(1);
		m_apertureSizeSlider->value(m_nApertureSize);
		m_apertureSizeSlider->align(FL_ALIGN_RIGHT);
		m_apertureSizeSlider->callback(cb_apertureSizeSlider);

		// install button AABB
		m_motionButton = new Fl_Light_Button(10, 375, 70, 25,"BVH");
		m_motionButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_motionButton->callback(cb_aabbButton);

		m_noiseInputSlider = new Fl_Value_Slider(10, 405, 180, 20, "Noise Input");
		m_noiseInputSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_noiseInputSlider->type(FL_HOR_NICE_SLIDER);
        m_noiseInputSlider->labelfont(FL_COURIER);
        m_noiseInputSlider->labelsize(12);
		m_noiseInputSlider->minimum(1);
		m_noiseInputSlider->maximum(10);
		m_noiseInputSlider->step(1);
		m_noiseInputSlider->value(m_nNoiseInput);
		m_noiseInputSlider->align(FL_ALIGN_RIGHT);
		m_noiseInputSlider->callback(cb_noiseInputSlider); // Need to change
		
		m_noiseScaleSlider = new Fl_Value_Slider(10, 430, 180, 20, "Noise Scale");
		m_noiseScaleSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_noiseScaleSlider->type(FL_HOR_NICE_SLIDER);
        m_noiseScaleSlider->labelfont(FL_COURIER);
        m_noiseScaleSlider->labelsize(12);
		m_noiseScaleSlider->minimum(2);
		m_noiseScaleSlider->maximum(5);
		m_noiseScaleSlider->step(1);
		m_noiseScaleSlider->value(m_nNoiseScale);
		m_noiseScaleSlider->align(FL_ALIGN_RIGHT);
		m_noiseScaleSlider->callback(cb_noiseScaleSlider); // Need to change

		m_noiseTextureButton = new Fl_Light_Button(10, 455, 160, 25, "Noise Texture");
		m_noiseTextureButton->user_data((void*)(this));   // record self to be used by static callback functions
		m_noiseTextureButton->callback(cb_noiseTextureButton); // Need to change
		m_noiseTextureButton->deactivate();

		m_calNoiseButton = new Fl_Button(180, 455, 160, 25, "Generate Noise");
		m_calNoiseButton->user_data((void*)(this));
		m_calNoiseButton->callback(cb_generateNoiseButton);

		m_renderButton = new Fl_Button(270, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(270, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}