// '/usr/bin/g++ -O2 `root-config --cflags --libs`' root_random_by_function.cc

#include "TROOT.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGaxis.h"
#include "TCut.h"
#include "TLine.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TRandom.h"
#include "TPostScript.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TColor.h"
#include "TNtuple.h"
#include "TLeaf.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
using namespace std;


int main(int argc, char** argv)
{    
  //-------------------------------------------------------------------------//
  const int DICE = 1000000;

  const double XMIN = 0;
  const double XMAX = 7;
  const double BIN_WIDTH = 0.01;

  char *function = "TMath::Exp(-0.88 * x) * TMath::SinH(TMath::Sqrt(2.0 * x))";
  //-------------------------------------------------------------------------//

  // set canvas style
  //
  gROOT  -> SetStyle("Plain");
  gStyle -> SetStatFont(42);
  gStyle -> SetOptStat(0);
  gStyle -> SetLabelFont(42,"XY");
  gStyle -> SetTitleFont(42, "XY");
  gStyle -> SetTitleSize(0.05, "XY");
  gStyle -> SetTitleOffset(1.0, "X");
  gStyle -> SetTitleOffset(1.0, "Y");

  TCanvas *c1 = new TCanvas("c1", "Title", 0, 0, 800, 600);
  c1 -> SetFillStyle(4001);
  c1 -> SetBottomMargin(0.12);
  c1 -> SetLeftMargin(0.12);
  c1 -> SetLogy();

  // make histgram
  //
  int bin = (int)((XMAX - XMIN) / BIN_WIDTH);
  TH1D *h1 = new TH1D("h1", "", bin, XMIN, XMAX);
  h1 -> SetXTitle("X");
  h1 -> SetYTitle("Y");

  // make function
  //
  TF1 *f1 = new TF1("f1", function, XMIN, XMAX);

  // get (x, y) of function
  //
  double x[bin], y[bin], integral = 0;
  for (int i = 1; i <= bin; i++) {
    x[i] = (i-0.5) * BIN_WIDTH + XMIN;
    y[i] = f1 -> Eval(x[i]);
    integral += y[i];
  }

  // get integral for each x
  //
  double Y[bin];
  for (int i = 0; i < bin; i++) {
    Y[i] = 0;
    for (int j = 0; j <= i; j++) {
      Y[i] += y[j];

    }
  }

  // get random
  //
  double x0;
  double random1;
  int k=0;
  for (int i = 0; i < DICE; i++) {
    random1 = Y[bin-1] * gRandom -> Uniform();

    for (int j = 0; j < bin; j++) {
      if (Y[j] < random1 && random1 < Y[j+1]) {
	x0 = x[j];
	break;
      }
    }
#if 0
    if(i % 4 == 0){
      cout << "# " <<  endl;
      cout << "# " << k << endl;
      k++;
    }
#endif
    //    cout << x0 << endl;
    h1 -> Fill(x0);
  }

  // draw function
  //
  h1 -> SetLineColor(4);
  h1 -> Draw();

  // draw graph
  //
  for (int i = 0; i < bin; i++) {
    y[i] *= DICE / integral;
  }
  TGraph* graph = new TGraph(bin, x, y);
  graph -> SetLineColor(2);
  graph -> SetLineWidth(2);
  graph -> Draw("");

  // make picture
  //
  char *ps_file = "picture_random.ps";
  c1 -> Print(ps_file);
  gSystem -> Exec(Form("gv %s", ps_file));

  return 0;
}

