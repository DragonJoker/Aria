#include "LayeredPanel.hpp"

namespace aria
{
	LayeredPanel::LayeredPanel( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel{ parent, wxID_ANY, position, size }
	{
		Bind( wxEVT_SIZING
			, [this]( wxSizeEvent & )
			{
				if ( m_current )
				{
					auto mySize = GetClientSize();
					m_current->SetSize( mySize );
				}
			} );
		Bind( wxEVT_SIZE
			, [this]( wxSizeEvent & )
			{
				if ( m_current )
				{
					auto mySize = GetClientSize();
					m_current->SetSize( mySize );
				}
			} );
	}

	void LayeredPanel::addLayer( wxPanel * panel )
	{
		m_panels.push_back( panel );
		panel->Hide();
	}

	void LayeredPanel::showLayer( size_t index )
	{
		if ( index != m_layer || !m_current )
		{
			auto size = GetClientSize();
			hideLayers();
			assert( m_panels.size() > index );
			m_current = m_panels[index];
			m_current->SetSize( size );
			m_current->Show();
			m_layer = index;
		}

		Update();
	}

	void LayeredPanel::hideLayers()
	{
		if ( m_current )
		{
			m_current->Hide();
			m_current = nullptr;
			m_layer = ~( 0u );
		}

		Update();
	}

	bool LayeredPanel::isLayerShown( size_t index )const
	{
		return m_current
			&& m_layer == index;
	}
}
