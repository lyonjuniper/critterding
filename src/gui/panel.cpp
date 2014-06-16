#include "panel.h"

Panel::Panel( bool resizable ) : BeWidgetContainer(resizable)
{
	m_active = false;
	isPanel = true;
	isTouchable = true;
	isTransparant = true;
	isAlwaysOnTop = false;
	
	zaxis = 0;
	if ( m_isresizable )
	{
		BeWidgetPtr w = addWidgetButton( "resizebutton", Vector2i( 0, 0 ), Vector2i(10, 10), "", Vector2i(0, 0), BeCommand(""), EVENT_NOREPEAT, 0 );
		w->isAlignedRight = true;
		w->isAlignedBottom = true;
	}
}

void Panel::draw()
{
	if (m_active)
	{
		drawBackground();
		drawBorders();
		drawChildren();
	}
}

void Panel::drawBackground()
{
	if (isTransparant)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	glPushMatrix();
	glColor4f(0.05f, 0.05f, 0.05f, 0.75f);
// 	glTranslatef(0.5f, 0.5f, 0.0f); // pixel precision offset
	glBegin(GL_QUADS);
		glVertex2i(m_absposition.m_x,         m_absposition.m_y+m_dimensions.m_y);
		glVertex2i(m_absposition.m_x,         m_absposition.m_y);
		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y);
		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y+m_dimensions.m_y);
	glEnd();
	glPopMatrix();

	if (isTransparant)
		glDisable(GL_BLEND);
}

void Panel::drawBorders()
{
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
// 	glTranslatef(0.5f, 0.5f, 0.0f); // pixel precision offset
	glBegin(GL_LINES);
		glVertex2i(m_absposition.m_x,         m_absposition.m_y+m_dimensions.m_y);
		glVertex2i(m_absposition.m_x,         m_absposition.m_y);

		glVertex2i(m_absposition.m_x,         m_absposition.m_y);
		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y);

		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y);
		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y+m_dimensions.m_y);

		glVertex2i(m_absposition.m_x+m_dimensions.m_x, m_absposition.m_y+m_dimensions.m_y);
		glVertex2i(m_absposition.m_x,         m_absposition.m_y+m_dimensions.m_y);
	glEnd();
	glPopMatrix();
}
