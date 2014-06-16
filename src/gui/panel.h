#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "container.h"

class Panel : public BeWidgetContainer
{
	public:
		Panel( bool resizable );
		virtual ~Panel() {};
		void		draw();
		unsigned int	zaxis;
		bool		isAlwaysOnTop;

		// autofocus widgetpointer
		BeWidgetPtr 		m_autofocuswidget;

	protected:
		virtual void	drawBackground();
		virtual void	drawBorders();

	private:

};

#endif
