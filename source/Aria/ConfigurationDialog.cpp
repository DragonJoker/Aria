#include "ConfigurationDialog.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/dirctrl.h>
#include <wx/filectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>
#pragma warning( pop )

namespace aria
{
	namespace config
	{
		enum ID
		{
			eID_OK,
			eID_CANCEL,
		};

		static constexpr int MinWidth = 600;
		static constexpr int MinHeight = 25;

		template< typename PickerT >
		static void addPickerField( wxWindow & parent
			, wxSizer & sizer
			, wxString const & name
			, wxString const & tip
			, wxFileName & value
			, wxEventTypeTag< wxFileDirPickerEvent > const & evt )
		{
			auto label = new wxStaticText{ &parent, wxID_ANY, name };
			sizer.Add( label, wxSizerFlags{} );

			auto tooltip = new wxToolTip{ tip };
			label->SetToolTip( tooltip );

			auto picker = new PickerT{ &parent, wxID_ANY, value.GetFullPath(), wxT( "Choose the " ) + name };
			picker->SetSize( wxSize( MinWidth, MinHeight ) );
			picker->SetMinSize( wxSize( MinWidth, MinHeight ) );
			sizer.Add( picker, wxSizerFlags{}.Border( wxBOTTOM, 5 ).FixedMinSize() );
			picker->Bind( evt
				, [&value, name]( wxFileDirPickerEvent & event )
				{
					auto result = event.GetPath();

					if ( !result.empty() )
					{
						value = event.GetPath();
					}
				} );
		}
	}

	void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value )
	{
		config::addPickerField< wxFilePickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_FILEPICKER_CHANGED );
	}

	void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value )
	{
		config::addPickerField< wxDirPickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_DIRPICKER_CHANGED );
	}

	ConfigurationDialog::ConfigurationDialog( wxWindow * parent
		, Plugin & plugin )
		: wxDialog{ parent, wxID_ANY, _( "Configuration" ), wxDefaultPosition, wxSize{ config::MinWidth, 400 } }
		, m_plugin{ plugin }
		, m_newPluginConfig{ m_plugin.createConfig() }
		, m_newConfig{ m_plugin.config }
		, m_config{ m_plugin.config }
	{
		m_newConfig.pluginConfig = m_newPluginConfig.get();
		auto fieldsSizer = new wxBoxSizer( wxVERTICAL );
		addDirField( *this
			, *fieldsSizer
			, wxT( "Tests folder" )
			, wxT( "The folder where the tests are located." )
			, m_newConfig.test );
		addDirField( *this
			, *fieldsSizer
			, wxT( "Analysis folder" )
			, wxT( "The folder where analysis results will be put." )
			, m_newConfig.work );
		addFileField( *this
			, *fieldsSizer
			, wxT( "Database file" )
			, wxT( "The SQLite database file that will hold the results." )
			, m_newConfig.database );
		m_newPluginConfig->fillDialog( *this, *fieldsSizer );

		auto okButton = new wxButton( this, config::eID_OK, _( "OK" ) );
		auto cancelButton = new wxButton( this, config::eID_CANCEL, _( "Cancel" ) );
		auto buttonsSizer = new wxBoxSizer( wxHORIZONTAL );
		buttonsSizer->Add( okButton );
		buttonsSizer->AddStretchSpacer();
		buttonsSizer->Add( cancelButton );

		auto windowSizer = new wxBoxSizer( wxVERTICAL );
		windowSizer->Add( fieldsSizer, wxSizerFlags{}.Border( wxALL, 5 ) );
		windowSizer->Add( buttonsSizer, wxSizerFlags{}.Border( wxALL, 5 ) );
		SetSizer( windowSizer );
		windowSizer->SetSizeHints( this );

		okButton->Bind( wxEVT_BUTTON
			, [this]( wxCommandEvent & )
			{
				m_config = m_newConfig;
				m_plugin.updateConfig( std::move( m_newPluginConfig ) );
				EndModal( wxID_OK );
			} );
		cancelButton->Bind( wxEVT_BUTTON
			, [this]( wxCommandEvent & )
			{
				EndModal( wxID_CANCEL );
			} );
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & )
			{
				EndModal( wxID_CANCEL );
			} );
	}
}
