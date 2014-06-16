#include "infostats.h"

Infostats::Infostats() : Panel(false)
{
	statsBuffer = Statsbuffer::Instance();

	hsp = 10;
	vsp = 13;

// 	active = true;
	isMovable = true;
	
	m_localposition.set(2, 24);
	
	m_dimensions.set(480, 75);
}

void Infostats::draw()
{
	if (m_active)
	{
// 		updateAbsPosition();
		drawBackground();
		drawBorders();

		unsigned int colWidth = (getPosition().m_x+m_dimensions.m_x-getPosition().m_x) / 4;
		unsigned int col1 = getPosition().m_x + colWidth;
		unsigned int col2 = col1 + colWidth;
		unsigned int col3 = col2 + colWidth;
		unsigned int col4 = col3 + colWidth;

		float linespacer = 0.0f;

		glColor3f(0.7f, 0.7f, 0.7f);
		glBegin(GL_LINES);
 			glVertex2f(col2, getPosition().m_y + linespacer);
 			glVertex2f(col2, (getPosition().m_y+getHeight()) - linespacer);
		glEnd();


		glColor3f(1.0f, 1.0f, 1.0f);
// 		glEnable(GL_TEXTURE_2D);

		// row counter
		unsigned int rc=1;

// 		statsSnapshot* ss = &statsBuffer->snapshots[statsBuffer->snapshots.size()-1];

		// HEADING

	// COLUMN 1
		Textprinter::Instance()->print(getPosition().m_x+hsp,	getPosition().m_y+vsp*rc,	"brain");

		rc++;
		Textprinter::Instance()->print(getPosition().m_x+hsp,	getPosition().m_y+vsp*rc,	"avg neurons:");
		if ( statsBuffer->current.critters > 0 )
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.neurons / statsBuffer->current.critters);
		else
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);

		rc++;
		Textprinter::Instance()->print(getPosition().m_x+hsp,	getPosition().m_y+vsp*rc,	"avg synapses:");
		if ( statsBuffer->current.critters > 0 )
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.synapses / statsBuffer->current.critters);
		else
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);

		rc++;
		Textprinter::Instance()->print(getPosition().m_x+hsp,	getPosition().m_y+vsp*rc,	"avg synapses/neuron:");
		if ( statsBuffer->current.neurons > 0 )
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.synapses / statsBuffer->current.neurons);
		else
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);

		rc++;
		Textprinter::Instance()->print(getPosition().m_x+hsp,	getPosition().m_y+vsp*rc,	"avg adam distance:");
		if ( statsBuffer->current.critters > 0 )
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.adamdistance / statsBuffer->current.critters);
		else
			Textprinter::Instance()->printR(col2-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);

	// COLUMN 2
		rc = 1;
		
		Textprinter::Instance()->print(col2+hsp,	getPosition().m_y+vsp*rc,	"body");

		rc++;
		Textprinter::Instance()->print(col2+hsp,	getPosition().m_y+vsp*rc,	"avg body parts:");
		if ( statsBuffer->current.critters > 0 )
			Textprinter::Instance()->printR(col4-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.bodyparts / statsBuffer->current.critters);
		else
			Textprinter::Instance()->printR(col4-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);

		rc++;
		Textprinter::Instance()->print(col2+hsp,	getPosition().m_y+vsp*rc,	"avg weight:");
		if ( statsBuffer->current.critters > 0 )
			Textprinter::Instance()->printR(col4-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", (float)statsBuffer->current.weight / statsBuffer->current.critters);
		else
			Textprinter::Instance()->printR(col4-hsp,	getPosition().m_y+vsp*rc,	"%1.2f", 0.0f);


// 		glDisable(GL_TEXTURE_2D);
	}
}

Infostats::~Infostats()
{
}
