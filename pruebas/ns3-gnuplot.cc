/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <fstream>
#include "ns3/gnuplot.h"

using namespace ns3;

int
main (int argc, char *argv[])
{
	Gnuplot plot;

	plot.SetTitle ("Ejemplo de GnuPlot");
	plot.AddDataset ( Gnuplot2dFunction ("Parabola 2x^2",
			"2*x**2") );

	Gnuplot2dDataset datos;
    datos.SetStyle (Gnuplot2dDataset::POINTS);
    datos.SetErrorBars (Gnuplot2dDataset::Y);
	for (int i = -5; i <= 5; i++)
		datos.Add(i, i*i, 2.0*i/10);
	plot.AddDataset (datos);

	std::ofstream fichero("prueba.plt");
	plot.GenerateOutput (fichero);
	fichero << "pause -1" << std::endl;
	fichero.close();
}