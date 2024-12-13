#include "TestResultsPanel.hpp"

#include "DiffImage.hpp"
#include "LayeredPanel.hpp"

#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

#include "xpms/save.xpm"

namespace aria
{
	//*********************************************************************************************

	namespace details
	{
		static wxImage loadRefImage( wxFileName const & folder
			, TestRun const & test )
		{
			return loadImage( folder / getReferenceFolder( test ) / getReferenceName( test ) );
		}

		static wxImage loadResultImage( wxFileName const & folder
			, TestRun const & test )
		{
			return loadImage( folder / getResultFolder( test ) / getResultName( test ) );
		}

		static wxImage getDiffImage( DiffMode mode
			, wxFileName const & refFile
			, wxFileName const & testFile )
		{
			if ( !wxFileExists( refFile.GetFullPath() ) )
			{
				wxLogError( "CompareImages - Image file doesn't exist: " + refFile.GetFullPath() );
				return wxImage{};
			}

			if ( !wxFileExists( testFile.GetFullPath() ) )
			{
				wxLogError( "CompareImages - Image file doesn't exist: " + testFile.GetFullPath() );
				return wxImage{};
			}

			wxImage result;
			return aria::getImageDiff( mode, refFile, testFile );
		}
	}

	//*********************************************************************************************

	class wxImagePanel
		: public wxPanel
	{
	public:
		explicit wxImagePanel( wxWindow * parent, bool maxSizeAtImageSize )
			: wxPanel{ parent }
			, m_maxSizeAtImageSize{ maxSizeAtImageSize }
		{
			SetBackgroundColour( BORDER_COLOUR );
			SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			Connect( wxEVT_PAINT
				, wxPaintEventHandler( wxImagePanel::paintEvent )
				, nullptr
				, this );
			Connect( wxEVT_SIZE
				, wxSizeEventHandler( wxImagePanel::sizeEvent )
				, nullptr
				, this );
		}

		void saveImage()
		{
			wxString strWildcard = _( "All supported files" );
			strWildcard += wxT( " (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.png;*.jpg|" );
			strWildcard += _( "BITMAP files" );
			strWildcard += wxT( " (*.bmp)|*.bmp|" );
			strWildcard += _( "GIF files" );
			strWildcard += wxT( " (*.gif)|*.gif|" );
			strWildcard += _( "JPEG files" );
			strWildcard += wxT( " (*.jpg)|*.jpg|" );
			strWildcard += _( "PNG files" );
			strWildcard += wxT( " (*.png)|*.png" );
			wxFileDialog dialog( this, _( "Please choose an image file name" ), wxEmptyString, wxEmptyString, strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( dialog.ShowModal() == wxID_OK )
			{
				m_source.SaveFile( dialog.GetPath() );
			}
		}

		void setImage( wxImage image )
		{
			m_source = std::move( image );
			m_current = {};

			if ( m_source.IsOk() && m_maxSizeAtImageSize )
			{
				SetMaxClientSize( m_source.GetSize() );
			}

			paintNow();
		}

		void paintNow()
		{
			wxClientDC dc( this );
			render( dc );
		}

	private:
		void render( wxDC & dc )
		{
			dc.Clear();

			if ( m_source.IsOk() )
			{
				auto size = GetClientSize();

				if ( !m_current.IsOk()
					|| size.GetWidth() != m_current.GetWidth()
					|| size.GetHeight() != m_current.GetHeight() )
				{
					auto ratio = float( m_source.GetHeight() ) / float( m_source.GetWidth() );
					auto w = float( size.GetWidth() );
					auto h = w * ratio;

					if ( h > float( size.GetHeight() ) )
					{
						h = float( size.GetHeight() );
						w = h / ratio;
					}

					m_current = m_source.ResampleBicubic( std::max( int( w ), 1 ), std::max( int( h ), 1 ) );
				}

				if ( m_current.IsOk() )
				{
					dc.DrawBitmap( m_current
						, ( size.GetWidth() - m_current.GetWidth() ) / 2
						, ( size.GetHeight() - m_current.GetHeight() ) / 2
						, false );
				}
			}
		}

		void paintEvent( wxPaintEvent & evt )
		{
			wxPaintDC dc( this );
			render( dc );
		}

		void sizeEvent( wxSizeEvent & evt )
		{
			paintNow();
		}

	private:
		wxImage m_source{};
		wxImage m_current{};
		bool m_maxSizeAtImageSize{ true };
	};

	//*********************************************************************************************

	TestResultsSideBySidePanel::TestResultsSideBySidePanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, Config const & config
		, std::array< wxImage, TestResultsPanel::eCount > & images )
		: wxPanel{ parent, id, {}, size }
		, m_config{ config }
		, m_images{ &images }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxImage saveImg{ save_xpm };

		wxArrayString sourceChoices;
		sourceChoices.push_back( _( "Source" ) );
		sourceChoices.push_back( _( "Raw Difference" ) );
		sourceChoices.push_back( _( "Logarithmic Difference" ) );
		sourceChoices.push_back( _( "ꟻLIP Difference" ) );

		auto refPanel = new wxPanel{ this };
		auto refTitle = new wxStaticText{ refPanel, wxID_ANY, _( "Reference" ), wxDefaultPosition, wxDefaultSize };
		auto refCombo = new wxComboBox{ refPanel, wxID_ANY, sourceChoices[0], wxDefaultPosition, wxDefaultSize, sourceChoices, wxCB_READONLY };
		auto refSave = new wxButton{ refPanel, wxID_ANY, _( "Save..." ), wxDefaultPosition, wxDefaultSize };
		refCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsSideBySidePanel::onRefSelect ), nullptr, this );
		refSave->Connect( wxEVT_BUTTON, wxCommandEventHandler( TestResultsSideBySidePanel::onRefSave ), nullptr, this );
		refSave->SetBitmap( saveImg );
		refSave->SetBackgroundColour( BORDER_COLOUR );
		refSave->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_ref = new wxImagePanel{ refPanel, true };
		wxBoxSizer * refComboSizer{ new wxBoxSizer{ wxHORIZONTAL } };
#if wxCHECK_VERSION( 3, 1, 5 )
		refComboSizer->Add( refTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ).CenterVertical() );
#else
		refComboSizer->Add( refTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ) );
#endif
		refComboSizer->Add( refCombo, wxSizerFlags{} );
		refComboSizer->Add( refSave, wxSizerFlags{} );
		wxBoxSizer * refSizer{ new wxBoxSizer{ wxVERTICAL } };
		refSizer->Add( refComboSizer, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		refSizer->Add( m_ref, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		refSizer->SetSizeHints( refPanel );
		refPanel->SetSizer( refSizer );

		auto resPanel = new wxPanel{ this };
		auto resTitle = new wxStaticText{ resPanel, wxID_ANY, _( "Test Result" ), wxDefaultPosition, wxDefaultSize };
		auto resCombo = new wxComboBox{ resPanel, wxID_ANY, sourceChoices[0], wxDefaultPosition, wxDefaultSize, sourceChoices, wxCB_READONLY };
		auto resSave = new wxButton{ resPanel, wxID_ANY, _( "Save..." ), wxDefaultPosition, wxDefaultSize };
		resCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsSideBySidePanel::onResSelect ), nullptr, this );
		resSave->Connect( wxEVT_BUTTON, wxCommandEventHandler( TestResultsSideBySidePanel::onResSave ), nullptr, this );
		resSave->SetBitmap( saveImg );
		resSave->SetBackgroundColour( BORDER_COLOUR );
		resSave->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_result = new wxImagePanel{ resPanel, true };
		wxBoxSizer * resComboSizer{ new wxBoxSizer{ wxHORIZONTAL } };
#if wxCHECK_VERSION( 3, 1, 5 )
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ).CenterVertical() );
#else
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ) );
#endif
		resComboSizer->Add( resCombo, wxSizerFlags{} );
		resComboSizer->Add( resSave, wxSizerFlags{} );
		wxBoxSizer * resSizer{ new wxBoxSizer{ wxVERTICAL } };
		resSizer->Add( resComboSizer, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		resSizer->Add( m_result, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		resSizer->SetSizeHints( resPanel );
		resPanel->SetSizer( resSizer );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
		sizer->Add( refPanel, wxSizerFlags{ 1 }.Expand().Border( wxALL, 0 ) );
		sizer->Add( resPanel, wxSizerFlags{ 1 }.Expand().Border( wxALL, 0 ) );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestResultsSideBySidePanel::refresh()
	{
		auto & test = *m_test;

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			m_currentRes = std::max( eSource, m_currentRes );
			loadRes( m_currentRes );
		}
		else
		{
			m_currentRef = eSource;
			m_currentRes = eNone;
			loadRes( m_currentRes );
		}

		m_currentRef = std::max( eSource, m_currentRef );
		loadRef( m_currentRef );

		Update();
	}

	void TestResultsSideBySidePanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
	}

	void TestResultsSideBySidePanel::loadRef( int index )
	{
		auto & test = *m_test;

		switch ( index )
		{
		case eSource:
			m_ref->setImage( m_images->at( TestResultsPanel::eReference ) );
			break;
		case eDiffRaw:
			if ( !m_images->at( TestResultsPanel::eDiffRefToResRaw ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffRefToResRaw ) = details::getDiffImage( DiffMode::eRaw
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_images->at( TestResultsPanel::eDiffRefToResRaw ) );
			break;
		case eDiffLog:
			if ( !m_images->at( TestResultsPanel::eDiffRefToResLog ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffRefToResLog ) = details::getDiffImage( DiffMode::eLogarithmic
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_images->at( TestResultsPanel::eDiffRefToResLog ) );
			break;
		case eDiffFlip:
			if ( !m_images->at( TestResultsPanel::eDiffRefToResFlip ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffRefToResLog ) = details::getDiffImage( DiffMode::eFlip
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_images->at( TestResultsPanel::eDiffRefToResLog ) );
			break;
		default:
			m_result->setImage( {} );
			index = eNone;
			break;
		}

		m_currentRef = ImgIndex( index );
	}

	void TestResultsSideBySidePanel::loadRes( int index )
	{
		auto & test = *m_test;

		switch ( index )
		{
		case eSource:
			m_result->setImage( m_images->at( TestResultsPanel::eResult ) );
			break;
		case eDiffRaw:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefRaw ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefRaw ) = details::getDiffImage( DiffMode::eRaw
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefRaw ) );
			break;
		case eDiffLog:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefLog ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefLog ) = details::getDiffImage( DiffMode::eLogarithmic
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefLog ) );
			break;
		case eDiffFlip:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefFlip ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefFlip ) = details::getDiffImage( DiffMode::eFlip
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefFlip ) );
			break;
		default:
			m_result->setImage( {} );
			index = eNone;
			break;
		}

		m_currentRes = ImgIndex( index );
	}

	void TestResultsSideBySidePanel::onRefSelect( wxCommandEvent & evt )
	{
		loadRef( evt.GetSelection() + 1 );
	}

	void TestResultsSideBySidePanel::onResSelect( wxCommandEvent & evt )
	{
		loadRes( evt.GetSelection() + 1 );
	}

	void TestResultsSideBySidePanel::onRefSave( wxCommandEvent & evt )
	{
		m_ref->saveImage();
	}

	void TestResultsSideBySidePanel::onResSave( wxCommandEvent & evt )
	{
		m_result->saveImage();
	}

	//*********************************************************************************************

	TestResultsFullSizePanel::TestResultsFullSizePanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, Config const & config
		, std::array< wxImage, TestResultsPanel::eCount > & images )
		: wxPanel{ parent, id, {}, size }
		, m_config{ config }
		, m_images{ &images }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxImage saveImg{ save_xpm };

		wxArrayString sourceChoices;
		sourceChoices.push_back( _( "Result" ) );
		sourceChoices.push_back( _( "Reference" ) );
		sourceChoices.push_back( _( "Raw Difference" ) );
		sourceChoices.push_back( _( "Logarithmic Difference" ) );
		sourceChoices.push_back( _( "ꟻLIP Difference" ) );

		auto resTitle = new wxStaticText{ this, wxID_ANY, _( "Test Result" ), wxDefaultPosition, wxDefaultSize };
		auto resCombo = new wxComboBox{ this, wxID_ANY, sourceChoices[0], wxDefaultPosition, wxDefaultSize, sourceChoices, wxCB_READONLY };
		auto resSave = new wxButton{ this, wxID_ANY, _( "Save..." ), wxDefaultPosition, wxDefaultSize };
		resCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsFullSizePanel::onSelect ), nullptr, this );
		resSave->Connect( wxEVT_BUTTON, wxCommandEventHandler( TestResultsFullSizePanel::onSave ), nullptr, this );
		resSave->SetBitmap( saveImg );
		resSave->SetBackgroundColour( BORDER_COLOUR );
		resSave->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_result = new wxImagePanel{ this, false };
		wxBoxSizer * resComboSizer{ new wxBoxSizer{ wxHORIZONTAL } };
#if wxCHECK_VERSION( 3, 1, 5 )
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ).CenterVertical() );
#else
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ) );
#endif
		resComboSizer->Add( resCombo, wxSizerFlags{} );
		resComboSizer->Add( resSave, wxSizerFlags{} );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->Add( resComboSizer, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		sizer->Add( m_result, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestResultsFullSizePanel::refresh()
	{
		auto & test = *m_test;

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			load( std::max( eResult, m_current ) );
		}
		else
		{
			load( eNone );
		}

		Update();
	}

	void TestResultsFullSizePanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
	}

	void TestResultsFullSizePanel::load( int index )
	{
		auto & test = *m_test;

		switch ( index )
		{
		case eResult:
			m_result->setImage( m_images->at( TestResultsPanel::eResult ) );
			break;
		case eReference:
			m_result->setImage( m_images->at( TestResultsPanel::eReference ) );
			break;
		case eDiffRaw:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefRaw ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefRaw ) = details::getDiffImage( DiffMode::eRaw
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefRaw ) );
			break;
		case eDiffLog:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefLog ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefLog ) = details::getDiffImage( DiffMode::eLogarithmic
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefLog ) );
			break;
		case eDiffFlip:
			if ( !m_images->at( TestResultsPanel::eDiffResToRefFlip ).IsOk() )
			{
				m_images->at( TestResultsPanel::eDiffResToRefFlip ) = details::getDiffImage( DiffMode::eFlip
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_images->at( TestResultsPanel::eDiffResToRefFlip ) );
			break;
		default:
			m_result->setImage( {} );
			index = eNone;
			break;
		}

		m_current = ImgIndex( index );
	}

	void TestResultsFullSizePanel::onSelect( wxCommandEvent & evt )
	{
		load( evt.GetSelection() + 1 );
	}

	void TestResultsFullSizePanel::onSave( wxCommandEvent & evt )
	{
		m_result->saveImage();
	}

	//*********************************************************************************************

	TestResultsPanel::TestResultsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, Config const & config )
		: wxPanel{ parent, id, {}, size }
		, m_config{ config }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxArrayString displayMode;
		displayMode.push_back( _( "Full Size" ) );
		displayMode.push_back( _( "Side By Side" ) );
		auto displaySelector = new wxRadioBox{ this, wxID_ANY, _( "Display Mode" ), wxDefaultPosition, wxDefaultSize, displayMode, 2, wxRA_SPECIFY_COLS };
		displaySelector->Connect( wxEVT_RADIOBOX, wxCommandEventHandler( TestResultsPanel::onDisplayMode ), nullptr, this );

		m_layers = new LayeredPanel{ this, wxDefaultPosition, wxDefaultSize };
		m_layers->SetBackgroundColour( BORDER_COLOUR );
		m_layers->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_fullSize = new TestResultsFullSizePanel{ m_layers, wxID_ANY, wxDefaultSize, config, m_images };
		m_layers->addLayer( m_fullSize );
		m_sideBySide = new TestResultsSideBySidePanel{ m_layers, wxID_ANY, wxDefaultSize, config, m_images };
		m_layers->addLayer( m_sideBySide );
		m_layers->showLayer( m_layer );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->Add( displaySelector, wxSizerFlags{ 0 }.Expand().Border( wxALL, 0 ) );
		sizer->Add( m_layers, wxSizerFlags{ 1 }.Expand().Border( wxALL, 0 ) );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestResultsPanel::refresh()
	{
		auto & test = *m_test;
		m_images[eDiffRefToResRaw] = {};
		m_images[eDiffRefToResLog] = {};
		m_images[eDiffRefToResFlip] = {};
		m_images[eDiffResToRefRaw] = {};
		m_images[eDiffResToRefLog] = {};
		m_images[eDiffResToRefFlip] = {};
		m_images[eReference] = details::loadRefImage( m_config.test, *test );

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			m_images[eResult] = details::loadResultImage( m_config.work, *test );
		}

		if ( m_layer == eSideBySide )
		{
			m_sideBySide->refresh();
		}
		else
		{
			m_fullSize->refresh();
		}
	}

	void TestResultsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		m_sideBySide->setTest( test );
		m_fullSize->setTest( test );
	}

	void TestResultsPanel::onDisplayMode( wxCommandEvent & evt )
	{
		m_layer = size_t( evt.GetInt() );

		if ( m_layer == eSideBySide )
		{
			m_sideBySide->refresh();
		}
		else
		{
			m_fullSize->refresh();
		}

		m_layers->showLayer( m_layer );
	}

	//*********************************************************************************************
}
