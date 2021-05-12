#include "ConfigurationDialog.hpp"

#include <wx/button.h>
#include <wx/filepicker.h>
#include <wx/dirctrl.h>
#include <wx/filectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>

namespace aria
{
	namespace
	{
		enum ID
		{
			eID_OK,
			eID_CANCEL,
		};

		static constexpr int MinWidth = 600;

		template< typename PickerT >
		void addPickerField( wxWindow * parent
			, wxSizer * sizer
			, wxString const & name
			, wxString const & tip
			, wxFileName & value
			, wxEventTypeTag< wxFileDirPickerEvent > const & evt )
		{
			auto label = new wxStaticText{ parent, wxID_ANY, name };
			sizer->Add( label, wxSizerFlags{} );

			auto tooltip = new wxToolTip{ tip };
			label->SetToolTip( tooltip );

			auto picker = new PickerT{ parent, wxID_ANY, value.GetFullPath(), wxT( "Choose the " ) + name };
			picker->SetSize( wxSize( MinWidth, 20 ) );
			picker->SetMinSize( wxSize( MinWidth, 20 ) );
			sizer->Add( picker, wxSizerFlags{}.Border( wxBOTTOM, 5 ).FixedMinSize() );
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

		void addFileField( wxWindow * parent
			, wxSizer * parentSizer
			, wxString const & name
			, wxString const & tip
			, wxFileName & value )
		{
			addPickerField< wxFilePickerCtrl >( parent
				, parentSizer
				, name
				, tip
				, value
				, wxEVT_FILEPICKER_CHANGED );
		}

		void addDirField( wxWindow * parent
			, wxSizer * parentSizer
			, wxString const & name
			, wxString const & tip
			, wxFileName & value )
		{
			addPickerField< wxDirPickerCtrl >( parent
				, parentSizer
				, name
				, tip
				, value
				, wxEVT_DIRPICKER_CHANGED );
		}
	}

	ConfigurationDialog::ConfigurationDialog( wxWindow * parent
		, Config & config )
		: wxDialog{ parent, wxID_ANY, _( "Configuration" ), wxDefaultPosition, wxSize{ MinWidth, 400 } }
		, m_config{ config }
		, m_newConfig{ config }
	{
		auto fieldsSizer = new wxBoxSizer( wxVERTICAL );
		addDirField( this
			, fieldsSizer
			, wxT( "Tests folder" )
			, wxT( "The folder where the tests are located." )
			, m_newConfig.test );
		addDirField( this
			, fieldsSizer
			, wxT( "Analysis folder" )
			, wxT( "The folder where analysis results will be put." )
			, m_newConfig.work );
		addFileField( this
			, fieldsSizer
			, wxT( "Database file" )
			, wxT( "The SQLite database file that will hold the results." )
			, m_newConfig.database );
		addFileField( this
			, fieldsSizer
			, wxT( "Test launcher executable" )
			, wxT( "The executable that will be used to run a single test." )
			, m_newConfig.launcher );
		addFileField( this
			, fieldsSizer
			, wxT( "Test viewer executable" )
			, wxT( "The executable that will be used to view a single test." )
			, m_newConfig.viewer );
		addFileField( this
			, fieldsSizer
			, wxT( "Engine main file" )
			, wxT( "The engine file that will be used to tell if a test is out of date, engine wise." )
			, m_newConfig.engine );

		auto okButton = new wxButton( this, eID_OK, _( "OK" ) );
		auto cancelButton = new wxButton( this, eID_CANCEL, _( "Cancel" ) );
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
