#include "TestResultsPanel.hpp"

#include "DiffImage.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"

#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/filename.h>

namespace aria
{
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

	class wxImagePanel
		: public wxPanel
	{
	public:
		explicit wxImagePanel( wxWindow * parent )
			: wxPanel{ parent }
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

		void setImage( wxImage image )
		{
			m_source = std::move( image );
			m_current = {};

			if ( m_source.IsOk() )
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
	};

	TestResultsPanel::TestResultsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, Config const & config )
		: wxPanel{ parent, id, {}, size }
		, m_config{ config }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxArrayString sourceChoices;
		sourceChoices.push_back( wxT( "Source" ) );
		sourceChoices.push_back( wxT( "Raw Difference" ) );
		sourceChoices.push_back( wxT( "Logarithmic Difference" ) );
		sourceChoices.push_back( wxT( "ꟻLIP Difference" ) );

		auto refPanel = new wxPanel{ this };
		auto refTitle = new wxStaticText{ refPanel, wxID_ANY, _( "Reference" ), wxDefaultPosition, wxDefaultSize };
		auto refCombo = new wxComboBox{ refPanel, wxID_ANY, sourceChoices[0], wxDefaultPosition, wxDefaultSize, sourceChoices, wxCB_READONLY };
		refCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsPanel::onRefSelect ), nullptr, this );
		m_ref = new wxImagePanel{ refPanel };
		wxBoxSizer * refComboSizer{ new wxBoxSizer{ wxHORIZONTAL } };
#if wxCHECK_VERSION( 3, 1, 5 )
		refComboSizer->Add( refTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ).CenterVertical() );
#else
		refComboSizer->Add( refTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ) );
#endif
		refComboSizer->Add( refCombo, wxSizerFlags{} );
		wxBoxSizer * refSizer{ new wxBoxSizer{ wxVERTICAL } };
		refSizer->Add( refComboSizer, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		refSizer->Add( m_ref, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		refSizer->SetSizeHints( refPanel );
		refPanel->SetSizer( refSizer );

		auto resPanel = new wxPanel{ this };
		auto resTitle = new wxStaticText{ resPanel, wxID_ANY, _( "Test Result" ), wxDefaultPosition, wxDefaultSize };
		auto resCombo = new wxComboBox{ resPanel, wxID_ANY, sourceChoices[0], wxDefaultPosition, wxDefaultSize, sourceChoices, wxCB_READONLY };
		resCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsPanel::onResSelect ), nullptr, this );
		m_result = new wxImagePanel{ resPanel };
		wxBoxSizer * resComboSizer{ new wxBoxSizer{ wxHORIZONTAL } };
#if wxCHECK_VERSION( 3, 1, 5 )
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ).CenterVertical() );
#else
		resComboSizer->Add( resTitle, wxSizerFlags{}.Border( wxRIGHT, 10 ) );
#endif
		resComboSizer->Add( resCombo, wxSizerFlags{} );
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

	void TestResultsPanel::refresh()
	{
		auto & test = *m_test;
		m_refToResImageRaw = {};
		m_refToResImageLog = {};
		m_refToResImageFlip = {};
		m_resToRefImageRaw = {};
		m_resToRefImageLog = {};
		m_resToRefImageFlip = {};

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			m_resImage = details::loadResultImage( m_config.work, *test );
			m_currentRes = std::max( eSource, m_currentRes );
			loadRes( m_currentRes );
		}
		else
		{
			m_currentRef = eSource;
			m_currentRes = eNone;
			loadRes( m_currentRes );
		}

		m_refImage = details::loadRefImage( m_config.test, *test );
		m_currentRef = std::max( eSource, m_currentRef );
		loadRef( m_currentRef );
	}

	void TestResultsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
	}

	void TestResultsPanel::loadRef( int index )
	{
		auto & test = *m_test;

		switch ( index )
		{
		case eSource:
			m_ref->setImage( m_refImage );
			break;
		case eDiffRaw:
			if ( !m_refToResImageRaw.IsOk() )
			{
				m_refToResImageRaw = details::getDiffImage( DiffMode::eRaw
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_refToResImageRaw );
			break;
		case eDiffLog:
			if ( !m_refToResImageLog.IsOk() )
			{
				m_refToResImageLog = details::getDiffImage( DiffMode::eLogarithmic
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_refToResImageLog );
			break;
		case eDiffFlip:
			if ( !m_refToResImageFlip.IsOk() )
			{
				m_refToResImageFlip = details::getDiffImage( DiffMode::eFlip
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
					, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			}
			m_ref->setImage( m_refToResImageFlip );
			break;
		default:
			m_result->setImage( {} );
			index = eNone;
			break;
		}

		m_currentRef = ImgIndex( index );
	}

	void TestResultsPanel::loadRes( int index )
	{
		auto & test = *m_test;

		switch ( index )
		{
		case eSource:
			m_result->setImage( m_resImage );
			break;
		case eDiffRaw:
			if ( !m_resToRefImageRaw.IsOk() )
			{
				m_resToRefImageRaw = details::getDiffImage( DiffMode::eRaw
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_resToRefImageRaw );
			break;
		case eDiffLog:
			if ( !m_resToRefImageLog.IsOk() )
			{
				m_resToRefImageLog = details::getDiffImage( DiffMode::eLogarithmic
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_resToRefImageLog );
			break;
		case eDiffFlip:
			if ( !m_resToRefImageFlip.IsOk() )
			{
				m_resToRefImageFlip = details::getDiffImage( DiffMode::eFlip
					, m_config.work / getResultFolder( *test ) / getResultName( *test )
					, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			}
			m_result->setImage( m_resToRefImageFlip );
			break;
		default:
			m_result->setImage( {} );
			index = eNone;
			break;
		}

		m_currentRes = ImgIndex( index );
	}

	void TestResultsPanel::onRefSelect( wxCommandEvent & evt )
	{
		loadRef( evt.GetSelection() + 1 );
	}

	void TestResultsPanel::onResSelect( wxCommandEvent & evt )
	{
		loadRes( evt.GetSelection() + 1 );
	}
}
