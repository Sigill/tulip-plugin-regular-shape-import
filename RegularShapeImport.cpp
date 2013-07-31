#include <tulip/ImportModule.h>
#include <tulip/StringCollection.h>
#include <tulip/TulipPluginHeaders.h>
#include <cmath>
#include <stdexcept>
#include <iostream>

using namespace std;
using namespace tlp;

const string PLUGIN_NAME("Regular shape import");

#define SHAPELIST "Circle;SinCircle;SpiralCircle"

namespace {
const char *ShapeList = SHAPELIST;

const char *paramHelp[] = {
	// 0
	HTML_HELP_OPEN() \
		HTML_HELP_DEF("Type", "String") \
		HTML_HELP_DEF("Values", SHAPELIST) \
		HTML_HELP_DEF("Default", "Circle") \
		HTML_HELP_BODY() \
		"The shape you want to build." \
		HTML_HELP_CLOSE(),

	// 1
	HTML_HELP_OPEN() \
		HTML_HELP_DEF("Type", "Unsigned int") \
		HTML_HELP_DEF("Default", "100") \
		HTML_HELP_BODY() \
		"Number of nodes." \
		HTML_HELP_CLOSE(),

	// 2
	HTML_HELP_OPEN() \
		HTML_HELP_DEF("Type", "Float") \
		HTML_HELP_DEF("Default", "1") \
		HTML_HELP_BODY() \
		"Radius." \
		HTML_HELP_CLOSE(),

	// 3
	HTML_HELP_OPEN() \
		HTML_HELP_DEF("Type", "Unsigned int") \
		HTML_HELP_DEF("Default", "10") \
		HTML_HELP_BODY() \
		"Frequency." \
		HTML_HELP_CLOSE(),

	// 4
	HTML_HELP_OPEN() \
		HTML_HELP_DEF("Type", "Float") \
		HTML_HELP_DEF("Default", "1.5") \
		HTML_HELP_BODY() \
		"Secondary radius." \
		HTML_HELP_CLOSE()
	};
}

class ShapeGenerator: public ImportModule {
public:
	PLUGININFORMATIONS(PLUGIN_NAME, "Cyrille Faucheux", "2013-02-07", "", "1.0", "Graph")

	ShapeGenerator(PluginContext *context) :
		ImportModule(context) {
			addInParameter< StringCollection > ("Shape",            paramHelp[0], ShapeList);
			addInParameter< unsigned int >     ("number of nodes",  paramHelp[1], "100");
			addInParameter< float >            ("radius",           paramHelp[2], "1");
			addInParameter< unsigned int >     ("frequency",        paramHelp[3], "10");
			addInParameter< float >            ("secondary radius", paramHelp[4], "0.5");
		}
	~ShapeGenerator() {}

	bool importGraph() {
		StringCollection shape;
		unsigned int number_of_nodes, frequency;
		float radius, secondary_radius;
		int i = 0;

		if(dataSet == NULL)
			throw std::runtime_error("No dataset provided.");

		if(!dataSet->get("Shape", shape))
			throw std::runtime_error(std::string("No \"Shape\" property provided."));

		if(!dataSet->get("number of nodes", number_of_nodes))
			throw std::runtime_error(std::string("No \"number of node\" property provided."));

		if(!dataSet->get("radius", radius))
			throw std::runtime_error(std::string("No \"radius\" property provided."));

		if(radius <= 0)
			throw std::runtime_error(std::string("Radius must be > 0."));

		if(shape.getCurrentString().compare("Circle") != 0) {
			if(!dataSet->get("frequency", frequency))
				throw std::runtime_error(std::string("No \"frequency\" property provided."));

			if(frequency <= 0)
				throw std::runtime_error(std::string("Frequency must be > 0."));


			if(!dataSet->get("secondary radius", secondary_radius))
				throw std::runtime_error(std::string("No \"secndary radius\" property provided."));

			if(secondary_radius <= 0)
				throw std::runtime_error(std::string("Secondary radius must be > 0."));
		}

		vector<node> nodes; nodes.reserve(number_of_nodes);
		graph->addNodes(number_of_nodes, nodes);
		//vector<pair<node, node> > ends; ends.reserve(number_of_nodes);
		//vector<edge> edges; edges.reserve(number_of_nodes);
		double x, y, z, theta;
		LayoutProperty *layout = graph->getProperty<LayoutProperty> ("viewLayout");
		vector<node>::const_iterator itNodes;

		if(shape.getCurrentString().compare("Circle") == 0) {
			z = 0.0;
			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)number_of_nodes;
				x = radius * cos(theta);
				y = radius * sin(theta);
				layout->setNodeValue(*itNodes, Coord(x, y, z));

				++i;
			}
		} else if(shape.getCurrentString().compare("SinCircle") == 0) {
			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)number_of_nodes;
				x = radius * cos(theta);
				y = radius * sin(theta);
				z = secondary_radius * sin(frequency * theta);
				layout->setNodeValue(*itNodes, Coord(x, y, z));

				++i;
			}
		} else if(shape.getCurrentString().compare("SpiralCircle") == 0) {
			double x1, y1;

			for(itNodes = nodes.begin(); itNodes < nodes.end(); ++itNodes) {
				theta = i * 2 * M_PI / (double)number_of_nodes;
				x1 = radius + secondary_radius * cos(frequency * theta);
				y1 = 0;
				z = secondary_radius * sin(frequency * theta);

				// Wrapping the spiral as a circle
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

PLUGIN(ShapeGenerator);
