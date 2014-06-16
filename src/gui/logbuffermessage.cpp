#include "logbuffermessage.h"

Logbuffermessage::Logbuffermessage( boost::shared_ptr<Logbuffer> logBuffer ) : Panel(false), m_logBuffer(logBuffer)
{
	isMovable = true;
	isAlwaysOnTop = true;
	m_active = true;
	
	vpadding = 10;
	hpadding = 10;

	m_localposition.set(2, 24);
}

void Logbuffermessage::draw()
{
	if ( m_active && !m_logBuffer->getMessages().empty() )
	{
// 		m_active = true;
		
		unsigned int height = 5;

		m_dimensions.set(m_logBuffer->getLongestLength() + ( hpadding*2 ), (15 * (m_logBuffer->getMessages().size()-1)) + height + ( vpadding*2 ));
		
	// draw background box and border
		drawBackground();
		drawBorders();

	// render text
		glColor3f(1.0f, 1.0f, 1.0f);
		for ( unsigned int i = 0; i < m_logBuffer->getMessages().size(); i++ )
			Textprinter::Instance()->print(m_absposition.m_x + hpadding, m_absposition.m_y + height + (i*15) + vpadding, m_logBuffer->getMessages()[i].getMsg().c_str());
	}
/*	else
		m_active = false;*/
}
