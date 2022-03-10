#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <String.h>
#include <vector>
#include <functional>

#include "widgets/widgets.h"
#include "widgets/widget_midi_index_list.h"

#include "gui_device.h"
#include "audio_engine/audio_device.h"



#define PARAM_VALUE_POS 53
#define PARAM_VALUE_POS_BROWSER 40



//------ guiDeviceSampler -------
guiDeviceSampler::guiDeviceSampler( Adafruit_SSD1306 *disp, 
                                    audioEngine * engine,
                                    enum audioEngine::AUDIO_ENGINE_DEVICES i_engine_sampler,
                                    const __FlashStringHelper *device, 
                                    const uint16_t x, const uint16_t y, 
                                    const uint16_t width, const uint16_t height) : guiDevice()
{
  m_display = disp;

  m_pos_x = x;
  m_pos_y = y;

  m_width  = width;
  m_height = height;

  m_id = 0;

  l_device = reinterpret_cast<const char *>(device); 

  m_param_y_pos = PARAM_VALUE_POS;
  m_widget_type  = widget::DEVICE;      


#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "create guiSampler: x(%d), y(%d), w(%d), h(%d)\n",
    m_pos_x, m_pos_y, m_width, m_height );
  Serial.print(str_);
#endif  

  audioDeviceSampler * engine_device = reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler ) );
  

  //param list
  m_param_list = new widgetList(m_display, m_pos_x, m_pos_y, m_width, m_height);


   


  //-------------------{ Sampler Main }-----------------------

  widgetParamFloat *param_f;

 // m_param_list->pushWidget( new widgetLabel( m_display, F("     ( Main )     ")) ); 
  m_param_list->pushWidget( new widgetLabel( m_display, F("-----{Main}-----")) ); 
  //volume
  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Volume"), F("Main_Volume"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolume,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));


  m_params.push_back( param_f );
  m_param_list->pushWidget(param_f);




  //-------------------{ Player Param }-----------------------
  //Pitch
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Pitch"), F("SRC_Pitch"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(80.0);
  param_f->setMin(-80.0);  
  param_f->setValueDefault(0);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setPitch,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f);

  //start
  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Start"), F("SRC_Start"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(95);
  param_f->setValueDefault(0);
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setStart,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f);


  //-------------------{ Player Envelop }-----------------------
  m_param_list->pushWidget( new widgetLabel( m_display, F("---{Envelope}---")) ); 
  //attack
  param_f = new widgetParamFloat( m_display, l_device, 
                                  F("Attack"), F("SRC_Attack"), 
                                  m_param_y_pos, widgetParamFloat::SECONDS );
  param_f->setMax(9999);
  param_f->setValueDefault(0);
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setAttack,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f);


  //hold
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Hold"), F("SRC_Hold"), 
                                  m_param_y_pos, widgetParamFloat::SECONDS );
  param_f->setMax(9999);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setHold,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 


  //decay
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Decay"), F("SRC_Decay"), 
                                  m_param_y_pos, widgetParamFloat::SECONDS );
  param_f->setMax(9999);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setDecay,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 


  //sustain
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Sustain"), F("SRC_Sustain"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(80);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setSustain,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 

  //sustain
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Release"), F("SRC_Release"), 
                                  m_param_y_pos, widgetParamFloat::SECONDS );
  param_f->setMax(9999);
  param_f->setValueDefault(250);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setRelease,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 



  // //length
  // param_f = new widgetParamFloat( m_display, l_device,
  //                                 F("Length"), F("SRC_Length"), 
  //                                 m_param_y_pos, widgetParamFloat::PERCENT );
  // param_f->setMax(100);
  // param_f->setValueDefault(100);  
  // param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setLength,
  //           reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
  //           std::placeholders::_1));

  // m_params.push_back( param_f );  
  //m_param_list->pushWidget(param_f); 




  //-------------------{ Channel Vol }-----------------------
  //CH 1
  m_param_list->pushWidget( new widgetLabel( m_display, F("----{Output}----")) ); 
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Vol_ch1"), F("SRC_Vol_ch1"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolCH1,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 

  //CH 2
  param_f = new widgetParamFloat( m_display, l_device,
                                  F("Vol_ch2"), F("SRC_Vol_ch2"), 
                                  m_param_y_pos, widgetParamFloat::PERCENT );
  param_f->setMax(100);
  param_f->setValueDefault(100);  
  param_f->setUpdateCallback(std::bind(&audioDeviceSampler::setVolCH2,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_f );  
  m_param_list->pushWidget(param_f); 





  //-------------------{ Browser }-----------------------
  m_param_list->pushWidget( new widgetLabel( m_display, F("----{Sample}----")) ); 

  widgetParamBrowser *param_b = new widgetParamBrowser( m_display, l_device,
                                                        F("Sample"), F("SRC_SampleName"), 
                                                        PARAM_VALUE_POS_BROWSER);

  param_b->setDir(F("/samples"));
  param_b->setUpdateCallback(std::bind(&audioDeviceSampler::openSample,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice( i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_b  );  
  m_param_list->pushWidget( param_b );   



  //-------------------{ Midi }-----------------------
  m_param_list->pushWidget( new widgetLabel( m_display, F("-----{midi}-----")) ); 
  //CH
  widgetParamInt *param_i = new widgetParamInt( m_display, l_device,
                                                F("CH"), F("SRC_MidiCH"), 
                                                m_param_y_pos );

  param_i->setMax(16);
  param_i->setMin(1);
  param_i->setValueDefault((i_engine_sampler+1));

  param_i->setUpdateCallback(std::bind(&audioDeviceSampler::setMidiCh,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_i  );  
  m_param_list->pushWidget( param_i );   

  //note min 
  widgetParamList *param_l = new widgetParamList( m_display, l_device,
                                                  F("Min"), F("SRC_MidiNoteMin"), 
                                                  m_param_y_pos, midi_index_list_with_number, MIDI_INDEX_LIST_NUMBER_LEN );

  param_l->setValueDefault(0);
  param_l->setOffset(1);

  param_l->setUpdateCallback(std::bind(&audioDeviceSampler::setNoteMin,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_l );  
  m_param_list->pushWidget( param_l );     

  //note max
  param_l =                new widgetParamList( m_display, l_device,
                                                F("Max"), F("SRC_MidiNoteMax"), 
                                                m_param_y_pos, midi_index_list_with_number, MIDI_INDEX_LIST_NUMBER_LEN );

  param_l->setValueDefault(0);
  param_l->setOffset(1);

  param_l->setUpdateCallback(std::bind(&audioDeviceSampler::setNoteMax,
            reinterpret_cast<audioDeviceSampler *>(engine->getDevice(  i_engine_sampler )),
            std::placeholders::_1));

  m_params.push_back( param_l );  
  m_param_list->pushWidget( param_l );     


  //-------------------{ Sampler Modulation Pot }-----------------------
  //----- PotA
  widgetParamList *param_pot_a       = new widgetParamList( m_display, l_device,
                                                            F("POTa ->"), F("SRC_POT_MOD_A"), 
                                                            m_param_y_pos-5 );

  param_pot_a->setValueDefault(0);

  param_pot_a->setUpdateCallback( std::bind(&audioDeviceSampler::modPotregParam,
                                                 engine_device,
                                                 0, std::placeholders::_1) );  

  //----- PotA Min
  widgetParamFloat *param_pot_a_min = new widgetParamFloat( m_display, l_device,
                                          F("POTa Min"), F("SRC_POT_MOD_A_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_a_min->setMax( 200);  
  param_pot_a_min->setMin(-200);
  param_pot_a_min->setValueDefault(0);  
  param_pot_a_min->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMin,
                                                    engine_device,
                                                    0, std::placeholders::_1) );  


  //----- PotA Max
  widgetParamFloat *param_pot_a_max = new widgetParamFloat( m_display, l_device,
                                          F("POTa Max"), F("SRC_POT_MOD_A_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_a_max->setMax( 200);  
  param_pot_a_max->setMin(-200);
  param_pot_a_max->setValueDefault(0);  
  param_pot_a_max->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMax,
                                                    engine_device,
                                                    0, std::placeholders::_1) );  


  //----- PotB
  widgetParamList *param_pot_b       = new widgetParamList( m_display, l_device,
                                                            F("POTb ->"), F("SRC_POT_MOD_B"), 
                                                            m_param_y_pos-5 );

  param_pot_b->setValueDefault(0);

  param_pot_b->setUpdateCallback( std::bind(&audioDeviceSampler::modPotregParam,
                                                 engine_device,
                                                 1, std::placeholders::_1) );  

  //----- PotB Min
  widgetParamFloat *param_pot_b_min = new widgetParamFloat( m_display, l_device,
                                          F("POTb Min"), F("SRC_POT_MOD_B_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_b_min->setMax( 200);  
  param_pot_b_min->setMin(-200);
  param_pot_b_min->setValueDefault(0);  
  param_pot_b_min->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMin,
                                                    engine_device,
                                                    1, std::placeholders::_1) );  


  //----- PotB Max
  widgetParamFloat *param_pot_b_max = new widgetParamFloat( m_display, l_device,
                                          F("POTb Max"), F("SRC_POT_MOD_B_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_b_max->setMax( 200);  
  param_pot_b_max->setMin(-200);
  param_pot_b_max->setValueDefault(0);  
  param_pot_b_max->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMax,
                                                    engine_device,
                                                    1, std::placeholders::_1) );  


  //----- PotC
  widgetParamList *param_pot_c       = new widgetParamList( m_display, l_device,
                                                            F("POTc ->"), F("SRC_POT_MOD_C"), 
                                                            m_param_y_pos-5 );

  param_pot_c->setValueDefault(0);

  param_pot_c->setUpdateCallback( std::bind(&audioDeviceSampler::modPotregParam,
                                                 engine_device,
                                                 2, std::placeholders::_1) );  

  //----- PotC Min
  widgetParamFloat *param_pot_c_min = new widgetParamFloat( m_display, l_device,
                                          F("POTc Min"), F("SRC_POT_MOD_C_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_c_min->setMax( 200);  
  param_pot_c_min->setMin(-200);
  param_pot_c_min->setValueDefault(0);  
  param_pot_c_min->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMin,
                                                    engine_device,
                                                    2, std::placeholders::_1) );  


  //----- PotC Max
  widgetParamFloat *param_pot_c_max = new widgetParamFloat( m_display, l_device,
                                          F("POTc Max"), F("SRC_POT_MOD_C_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_pot_c_max->setMax( 200);  
  param_pot_c_max->setMin(-200);
  param_pot_c_max->setValueDefault(0);  
  param_pot_c_max->setUpdateCallback( std::bind(&audioDeviceSampler::modPotsetMax,
                                                    engine_device,
                                                    2, std::placeholders::_1) );  




  //-------------------{ Sampler Modulation Midi }-----------------------
  //----- Midi CCa Reg
  widgetParamList *param_midi_cc_a  = new widgetParamList( m_display, l_device,
                                                            F("CCa ->"), F("SRC_MIDI_MOD_CCA"), 
                                                            m_param_y_pos-5 );

  param_midi_cc_a->setValueDefault(0);

  param_midi_cc_a->setUpdateCallback( std::bind(&audioDeviceSampler::modCCregParam,
                                                 engine_device,
                                                 0, std::placeholders::_1) );  


  //Midi CCa CH
  widgetParamList *param_midi_cc_a_ch = new widgetParamList( m_display, l_device,
                                                            F("CCa <-"), F("SRC_MIDI_MOD_CCA_CH"), 
                                                            m_param_y_pos-5, midi_cc_list_with_number, 120 );



  param_midi_cc_a_ch->setValueDefault(1);
  param_midi_cc_a_ch->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetCH,
                                                    engine_device,
                                                    0, std::placeholders::_1) );  


  //Midi CCa Min
  widgetParamFloat *param_midi_cc_a_min = new widgetParamFloat( m_display, l_device,
                                          F("CCa Min"), F("SRC_MIDI_MOD_CCA_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_a_min->setMax( 200);  
  param_midi_cc_a_min->setMin(-200);
  param_midi_cc_a_min->setValueDefault(0);  
  param_midi_cc_a_min->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMin,
                                                    engine_device,
                                                    0, std::placeholders::_1) );  


  //Midi CCa Max
  widgetParamFloat *param_midi_cc_a_max = new widgetParamFloat( m_display, l_device,
                                          F("CCa Max"), F("SRC_MIDI_MOD_CCA_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_a_max->setMax( 200);  
  param_midi_cc_a_max->setMin(-200);
  param_midi_cc_a_max->setValueDefault(0);  
  param_midi_cc_a_max->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMax,
                                                    engine_device,
                                                    0, std::placeholders::_1) );  

  //------------
  //Midi CCb Reg
  widgetParamList *param_midi_cc_b  = new widgetParamList( m_display, l_device,
                                                            F("CCb ->"), F("SRC_MIDI_MOD_CCB"), 
                                                            m_param_y_pos-5);

  param_midi_cc_b->setValueDefault(0);

  param_midi_cc_b->setUpdateCallback( std::bind(&audioDeviceSampler::modCCregParam,
                                                 engine_device,
                                                 1, std::placeholders::_1) );  


  //Midi CCb CH
  widgetParamList *param_midi_cc_b_ch = new widgetParamList( m_display, l_device,
                                                            F("CCb <-"), F("SRC_MIDI_MOD_CCB_CH"), 
                                                            m_param_y_pos-5, midi_cc_list_with_number, 120 );



  param_midi_cc_b_ch->setValueDefault(1);
  param_midi_cc_b_ch->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetCH,
                                                    engine_device,
                                                    1, std::placeholders::_1) );  


  //Midi CCa Min
  widgetParamFloat *param_midi_cc_b_min = new widgetParamFloat( m_display, l_device,
                                          F("CCb Min"), F("SRC_MIDI_MOD_CCB_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_b_min->setMax( 200);  
  param_midi_cc_b_min->setMin(-200);
  param_midi_cc_b_min->setValueDefault(0);  
  param_midi_cc_b_min->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMin,
                                                    engine_device,
                                                    1, std::placeholders::_1) );  


  //Midi CCa Max
  widgetParamFloat *param_midi_cc_b_max = new widgetParamFloat( m_display, l_device,
                                          F("CCb Max"), F("SRC_MIDI_MOD_CCB_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_b_max->setMax( 200);  
  param_midi_cc_b_max->setMin(-200);
  param_midi_cc_b_max->setValueDefault(0);  
  param_midi_cc_b_max->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMax,
                                                    engine_device,
                                                    1, std::placeholders::_1) );  


  //------------
  //Midi CCc Reg
  widgetParamList *param_midi_cc_c  = new widgetParamList( m_display, l_device,
                                                            F("CCc ->"), F("SRC_MIDI_MOD_CCC"), 
                                                            m_param_y_pos-5 );

  param_midi_cc_c->setValueDefault(0);

  param_midi_cc_c->setUpdateCallback( std::bind(&audioDeviceSampler::modCCregParam,
                                                 engine_device,
                                                 2, std::placeholders::_1) );  


  //Midi CCc CH
  widgetParamList *param_midi_cc_c_ch = new widgetParamList( m_display, l_device,
                                                            F("CCc <-"), F("SRC_MIDI_MOD_CCC_CH"), 
                                                            m_param_y_pos-5, midi_cc_list_with_number, 120 );



  param_midi_cc_c_ch->setValueDefault(1);
  param_midi_cc_c_ch->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetCH,
                                                    engine_device,
                                                    2, std::placeholders::_1) );  


  //Midi CCa Min
  widgetParamFloat *param_midi_cc_c_min = new widgetParamFloat( m_display, l_device,
                                          F("CCc Min"), F("SRC_MIDI_MOD_CCC_MIN"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_c_min->setMax( 200);  
  param_midi_cc_c_min->setMin(-200);
  param_midi_cc_c_min->setValueDefault(0);  
  param_midi_cc_c_min->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMin,
                                                    engine_device,
                                                    2, std::placeholders::_1) );  


  //Midi CCa Max
  widgetParamFloat *param_midi_cc_c_max = new widgetParamFloat( m_display, l_device,
                                          F("CCc Max"), F("SRC_MIDI_MOD_CCC_MAX"), 
                                          m_param_y_pos, widgetParamFloat::PERCENT );
  param_midi_cc_c_max->setMax( 200);  
  param_midi_cc_c_max->setMin(-200);
  param_midi_cc_c_max->setValueDefault(0);  
  param_midi_cc_c_max->setUpdateCallback( std::bind(&audioDeviceSampler::modCCsetMax,
                                                    engine_device,
                                                    2, std::placeholders::_1) );  


  //Set ModWidget Labels
  for(int i= audioDeviceSampler::MOD_DEST_NONE; i<audioDeviceSampler::MOD_DEST_NUM; i++){
    param_pot_a->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );
    param_pot_b->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );
    param_pot_c->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );

    param_midi_cc_a->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );
    param_midi_cc_b->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );
    param_midi_cc_c->pushString( engine_device->getParamString((audioDeviceSampler::mod_dest_t)(i)) );   
  }

  

  //-------------------{ Modulation GUI }-----------------------
  m_param_list->pushWidget( new widgetLabel( m_display, F("---{Mod Poti}---")) ); 
  //POTa
  m_params.push_back( param_pot_a );  
  m_param_list->pushWidget( param_pot_a );       

  //POTb
  m_params.push_back( param_pot_b );  
  m_param_list->pushWidget( param_pot_b );      

  //POTc
  m_params.push_back( param_pot_c );  
  m_param_list->pushWidget( param_pot_c );      


  //MinMax
  m_params.push_back( param_pot_a_min );  
  m_param_list->pushWidget( param_pot_a_min );    

  m_params.push_back( param_pot_a_max );  
  m_param_list->pushWidget( param_pot_a_max );    

  m_params.push_back( param_pot_b_min );  
  m_param_list->pushWidget( param_pot_b_min );    

  m_params.push_back( param_pot_b_max );  
  m_param_list->pushWidget( param_pot_b_max );    

  m_params.push_back( param_pot_c_min );  
  m_param_list->pushWidget( param_pot_c_min );    

  m_params.push_back( param_pot_c_max );  
  m_param_list->pushWidget( param_pot_c_max );    



  //Mod Midi
  m_param_list->pushWidget( new widgetLabel( m_display, F("---{Mod Midi}---")) ); 

  //Param Reg
  m_params.push_back( param_midi_cc_a );  
  m_param_list->pushWidget( param_midi_cc_a );       

  m_params.push_back( param_midi_cc_b );  
  m_param_list->pushWidget( param_midi_cc_b );       

  m_params.push_back( param_midi_cc_c );  
  m_param_list->pushWidget( param_midi_cc_c );    


  //Source
  m_params.push_back( param_midi_cc_a_ch );  
  m_param_list->pushWidget( param_midi_cc_a_ch );     

  m_params.push_back( param_midi_cc_b_ch );  
  m_param_list->pushWidget( param_midi_cc_b_ch );     
  
  m_params.push_back( param_midi_cc_c_ch );  
  m_param_list->pushWidget( param_midi_cc_c_ch );         


  //Min/Max
  m_params.push_back( param_midi_cc_a_min);  
  m_param_list->pushWidget( param_midi_cc_a_min );   
  m_params.push_back(param_midi_cc_a_max);
  m_param_list->pushWidget( param_midi_cc_a_max );   

  m_params.push_back( param_midi_cc_b_min);  
  m_param_list->pushWidget( param_midi_cc_b_min );  
  m_params.push_back( param_midi_cc_b_max);  
  m_param_list->pushWidget( param_midi_cc_b_max );   

  m_params.push_back( param_midi_cc_c_min );  
  m_param_list->pushWidget( param_midi_cc_c_min );  
  m_params.push_back( param_midi_cc_c_max );  
  m_param_list->pushWidget( param_midi_cc_c_max );   





  //push to gui list
  m_param_list->setIndex(0);

}








//------ guiDevice -------
void guiDevice::inc_rotary(bool inc)
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler inc: m_edit(%s), inc(%s)\n",
    (m_edit ? "true" : "false"),  (inc ? "true" : "false") );
  Serial.print(str_);
#endif  

  if(m_edit){
    widgetParam* w = reinterpret_cast<widgetParam* >(m_param_list->getActiveWidget());
    if(w==NULL){return;}
    if(!w->isWidgetType(widget::PARAM)){return;}   

    w->incValue(inc);
  }
  else{
    m_param_list->incIndex(inc);
  }
}

void guiDevice::rstIndex()
{
  m_param_list->setIndex(0);
}


void guiDevice::set_edit()
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler set: m_edit(%s)\n", (m_edit ? "true" : "false") );
  Serial.print(str_);
#endif  


  widgetParam* w = reinterpret_cast<widgetParam* >(m_param_list->getActiveWidget());
  if(w==NULL){return;}
  if(!w->isWidgetType(widget::PARAM)){return;}

  w->setEdit(m_edit);
}


void guiDevice::draw()
{

#if defined(DEBUG_GUI_DEVICE_SAMPLER) && defined(DEBUG_GUI_DEVICE)
  sprintf(str_, "gui Sampler force draw: m_active(%s) m_edit(%s)\n", 
   (m_active ? "true" : "false"), (m_edit ? "true" : "false") );
  Serial.print(str_);
#endif  

  m_param_list->setActive(m_active);

  if(m_edit){
    widget* w = m_param_list->getActiveWidget();
    if(w==NULL){return;}  
    w->forceDraw();  
  }
  else{
    m_param_list->forceDraw();
  }
  
}


void guiDevice::read_handler(patchHandler *m_handler)
{

#ifdef  DEBUG_GUI_DEVICE
  sprintf(str_, "gui device (%s): readHandler, m_param.size(%d)\n", l_device, m_params.size());
  Serial.print(str_);
#endif  

  for(auto pa : m_params){

    if       (pa->isParamType(widgetParam::FLOAT)){
      widgetParamFloat *p = reinterpret_cast<widgetParamFloat *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue(val);
      }

      //udate guie
      p->triggerCallback();
    }

    else if (pa->isParamType(widgetParam::INT)){
      widgetParamInt *p = reinterpret_cast<widgetParamInt *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue((int)val);
      }

      //udate guie
      p->triggerCallback();
    }

    else if (pa->isParamType(widgetParam::LIST)){
      widgetParamList *p = reinterpret_cast<widgetParamList *>(pa); 
      float val;
      if(!m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->useDefaultVal();
      }
      else{
        p->setValue((int)val);
      }

      //udate guie
      p->triggerCallback();
    }    

    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val;
      if(m_handler->getParamValue(p->getLDevice(), p->getLParam(), val)){
        p->setValue(val);
      }

      p->triggerCallback();
    }    
  }
}

void guiDevice::save_handler(patchHandler *m_handler)
{

#ifdef  DEBUG_GUI_DEVICE
  sprintf(str_, "gui device (%s): saveHandler\n", l_device);
  Serial.print(str_);
#endif  

  for(auto pa: m_params){
    if      (pa->isParamType(widgetParam::FLOAT)){
      widgetParamFloat *p = reinterpret_cast<widgetParamFloat *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), p->getValue());
    }

    else if (pa->isParamType(widgetParam::INT)){
      widgetParamInt *p = reinterpret_cast<widgetParamInt *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), (float)p->getValue());
    }

    else if (pa->isParamType(widgetParam::LIST)){
      widgetParamList *p = reinterpret_cast<widgetParamList *>(pa); 
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), (float)p->getValue());
    }

    else if(pa->isParamType(widgetParam::BROWSER)){
      widgetParamBrowser * p = reinterpret_cast<widgetParamBrowser *>(pa); 
      String val = p->getValue();
      m_handler->saveParamValue(p->getLDevice(), p->getLParam(), val);
    }
  }  
}
