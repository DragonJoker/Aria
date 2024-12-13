#include "ConfigurationDialog.hpp"

#include <AriaLib/Plugin.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/dirctrl.h>
#include <wx/filectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	namespace edtconfig
	{
		enum ID
		{
			eID_OK,
			eID_CANCEL,
		};

		static constexpr int MinWidth = 600;
	}

	ConfigurationDialog::ConfigurationDialog( wxWindow * parent
		, Plugin & plugin )
		: wxDialog{ parent, wxID_ANY, _( "Edit Configuration" ), wxDefaultPosition, wxSize{ edtconfig::MinWidth, 400 } }
		, m_plugin{ plugin }
		, m_newPluginConfig{ m_plugin.createConfig() }
		, m_newConfig{ m_plugin.config }
		, m_config{ m_plugin.config }
	{
		m_newConfig.pluginConfig = m_newPluginConfig.get();
		auto fieldsSizer = new wxBoxSizer( wxVERTICAL );

		auto cont = new wxStaticBox{ this, wxID_ANY, _( "Base" ) };
		auto contSizer = new wxBoxSizer( wxVERTICAL );
		auto contFieldsSizer = new wxBoxSizer( wxVERTICAL );
		addDirField( *cont
			, *contFieldsSizer
			, _( "Tests folder" )
			, _( "The folder where the tests are located." )
			, m_newConfig.test
			, 10 );
		addDirField( *cont
			, *contFieldsSizer
			, _( "Analysis folder" )
			, _( "The folder where analysis results will be put." )
			, m_newConfig.work );
		addFileField( *cont
			, *contFieldsSizer
			, _( "Database file" )
			, _( "The SQLite database file that will hold the results." )
			, m_newConfig.database );
		contSizer->Add( contFieldsSizer, wxSizerFlags{}.Expand().Border( wxALL, 10 ) );
		cont->SetSizer( contSizer );
		contSizer->SetSizeHints( cont );
		fieldsSizer->Add( cont, wxSizerFlags{}.Expand() );

		m_newPluginConfig->fillDialog( *this, *fieldsSizer );

		auto okButton = new wxButton( this, edtconfig::eID_OK, _( "OK" ) );
		auto cancelButton = new wxButton( this, edtconfig::eID_CANCEL, _( "Cancel" ) );
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
