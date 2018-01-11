/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */


//TO-DO: I should almost certainly use a comboBox gui element to select between nonlinearities!


#include "nonlin_spike.h"
#include <iostream>
#include <main_window.h>

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new NonlinSpike();
}

static DefaultGUIModel::variable_t vars[] = {
  {
    "Gain", " ",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "Shift", " ",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
  {
    "Bias", " ",
    DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
  },
   {
    "x in", " ",
    DefaultGUIModel::INPUT,
   },
   {
    "spike out", " ",
    DefaultGUIModel::OUTPUT,
   },
   {
    "prob out", " ",
    DefaultGUIModel::OUTPUT,
   },

};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

NonlinSpike::NonlinSpike(void)
  : DefaultGUIModel("NonlinSpike with Custom GUI", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>NonlinSpike:</b><br>QWhatsThis description.</p>");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  initParameters();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

NonlinSpike::~NonlinSpike(void)
{
}

void
NonlinSpike::execute(void)
{

  double x = input(0);
  double z; 
  int spike;


  switch (nlmode)
  {
  	case EXP: z = bias+exp(x*gain+shft);
				break;
	case SOFTLOG: z = log(bias+exp(x*gain+shft));
				break;
	default: z = (x*gain+shft)+bias;
  }

  if (z<0) {z=0;};
  if (z>1) {z=1;};

  std::uniform_real_distribution<double> unif(0, 1);
  spike = (unif(gen) < z) ? 1 : 0;

  output(0) = spike;
  output(1) = z;
  return;
}

void
NonlinSpike::initParameters(void)
{
  some_parameter = 0;
  some_state = 0;

  gain=1e-3;
  shft=0;
  bias=0;

  nlmode=EXP;

  gen.seed(rd());
}

void
NonlinSpike::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setParameter("Gain (*1e-3)", gain/1e-3);
      setParameter("Shift", shft);      
	  setParameter("Bias", bias);
      break;

    case MODIFY:
      gain = getParameter("Gain").toDouble()*1e-3;
      shft = getParameter("Shift").toDouble();
      bias = getParameter("Bias").toDouble();
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}

void
NonlinSpike::customizeGUI(void)
{
  QGridLayout* customlayout = DefaultGUIModel::getLayout();

  QGroupBox* button_group = new QGroupBox;

  QPushButton* abutton = new QPushButton("Button A");
  QPushButton* bbutton = new QPushButton("Button B");
  QHBoxLayout* button_layout = new QHBoxLayout;
  button_group->setLayout(button_layout);
  button_layout->addWidget(abutton);
  button_layout->addWidget(bbutton);
  QObject::connect(abutton, SIGNAL(clicked()), this, SLOT(aBttn_event()));
  QObject::connect(bbutton, SIGNAL(clicked()), this, SLOT(bBttn_event()));

  customlayout->addWidget(button_group, 0, 0);
  setLayout(customlayout);
}

// functions designated as Qt slots are implemented as regular C++ functions
void
NonlinSpike::aBttn_event(void)
{
}

void
NonlinSpike::bBttn_event(void)
{
}
