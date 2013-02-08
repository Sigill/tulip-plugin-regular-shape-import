#include <tulip/TulipPlugin.h>
#include <cmath>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace tlp;

const string PLUGIN_NAME("Regular shape import");

void displayMessage(string message, PluginProgress* pp)
{
	string m = "(" + PLUGIN_NAME + ") " + message;
	std::cerr << m << std::endl;
	if(pp) pp->setError(m);
}

#define SHAPELIST "Circle;SinCircle;SpiralCircle"

namespace {
const char *ShapeList = SHAPELIST;

const char *paramHelp[] = {
	HTML_HELP_OPEN()
		HTML_HELP_DEF("Type", "String")
		HTML_HELP_DEF("Values", SHAPELIST)
		HTML_HELP_DEF("Default", "Circle")
		HTML_HELP_BODY()
		"The shape you want to build."
		HTML_HELP_CLOSE()
	};
}

class ShapeGenerator: public ImportModule {
public:
	ShapeGenerator(AlgorithmContext context) :
		ImportModule(context) {
			addParameter< StringCollection >("Shape", paramHelp[0], ShapeList);
		}
	~ShapeGenerator() {}

	bool importGraph() {
		StringCollection shape;
		int i = 0;

		if(dataSet == NULL)
			throw std::runtime_error("No dataset provided.");

		if(!dataSet->get("Shape", shape))
			throw std::runtime_error(std::string("No \"Shape\" property provided."));

		const int count = 100;

		vector<node> nodes; nodes.reserve(count);
		graph->addNodes(count, nodes);
		//vector<pair<node, node> > ends; ends.reserve(count);
		//vector<edge> edges; edges.reserve(count);
		double x, y, z, theta;
		LayoutProperty *layout = graph->getProperty<LayoutProperty> ("viewLayout");
		vector<node>::const_iterator itNodes;

		if(shape.getCurrentString().compare("Circle") == 0) {
			z = 0.0;
			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)count;
				x = 25 * cos(theta);
				y = 25 * sin(theta);
				layout->setNodeValue(*itNodes, Coord(x, y, z));

				++i;
			}
		} else if(shape.getCurrentString().compare("SinCircle") == 0) {
			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)count;
				x = 25 * cos(theta);
				y = 25 * sin(theta);
				z = 10 * sin(5 * theta);
				layout->setNodeValue(*itNodes, Coord(x, y, z));

				++i;
			}
		} else if(shape.getCurrentString().compare("SpiralCircle") == 0) {
			double x1, y1;

			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)count;
				x1 = 25 + 5 * cos(10 * theta);
				y1 = 0;
				z = 5 * sin(10 * theta);
				
				x = x1 * cos(theta) + y1 * sin(theta);
				y = -x1 * sin(theta) + y1 * cos(theta);
				layout->setNodeValue(*itNodes, Coord(x, y, z));

				++i;
			}
		}

		//graph->addEdges(ends, edges);

		return true;
	}
};

IMPORTPLUGINOFGROUP(ShapeGenerator, PLUGIN_NAME, "Cyrille Faucheux", "07/02/2013", "", "1.0", "Graphs")