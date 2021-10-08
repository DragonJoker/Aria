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
	namespace
	{
		wxImage loadImage( wxFileName const & filePath )
		{
			if ( !wxFileExists( filePath.GetFullPath() ) )
			{
				return wxImage{};
			}

			wxImage result{ filePath.GetFullPath() };

			if ( result.IsOk() )
			{
				return result;
			}

			return wxImage{};
		}
		
		wxImage loadRefImage( wxFileName const & folder
			, TestRun const & test )
		{
			return loadImage( folder / getReferenceFolder( test ) / getReferenceName( test ) );
		}

		wxImage loadResultImage( wxFileName const & folder
			, TestRun const & test )
		{
			return loadImage( folder / getResultFolder( test ) / getResultName( test ) );
		}

		wxImage compareImages( wxFileName const & refFile
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

			wxImage reference = loadImage( refFile );
			wxImage toTest = loadImage( testFile );

			if ( toTest.GetSize() != reference.GetSize() )
			{
				wxLogError( "CompareImages - Images dimensions don't match: " + testFile.GetFullPath() );
				return wxImage{};
			}

			wxImage result;
			aria::compareImages( reference, toTest, result );
			return result;
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
					auto ratio = m_source.GetHeight() / float( m_source.GetWidth() );
					auto w = size.GetWidth();
					auto h = w * ratio;

					if ( h > size.GetHeight() )
					{
						h = size.GetHeight();
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

		auto refPanel = new wxPanel{ this };
		wxArrayString refChoices;
		refChoices.push_back( wxT( "Reference" ) );
		refChoices.push_back( wxT( "Difference" ) );
		auto refCombo = new wxComboBox{ refPanel, wxID_ANY, refChoices[0], wxDefaultPosition, wxDefaultSize, refChoices, wxCB_READONLY };
		refCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsPanel::onRefSelect ), nullptr, this );
		m_ref = new wxImagePanel{ refPanel };
		wxBoxSizer * refSizer{ new wxBoxSizer{ wxVERTICAL } };
		refSizer->Add( refCombo, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		refSizer->Add( m_ref, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		refSizer->SetSizeHints( refPanel );
		refPanel->SetSizer( refSizer );

		auto resPanel = new wxPanel{ this };
		wxArrayString resChoices;
		resChoices.push_back( wxT( "Test Result" ) );
		resChoices.push_back( wxT( "Difference" ) );
		auto resCombo = new wxComboBox{ resPanel, wxID_ANY, resChoices[0], wxDefaultPosition, wxDefaultSize, resChoices, wxCB_READONLY };
		resCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestResultsPanel::onResSelect ), nullptr, this );
		m_result = new wxImagePanel{ resPanel };
		wxBoxSizer * resSizer{ new wxBoxSizer{ wxVERTICAL } };
		resSizer->Add( resCombo, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
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

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			m_resImage = loadResultImage( m_config.work, *test );
			m_refToResImage = compareImages( m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
				, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			m_resToRefImage = compareImages( m_config.work / getResultFolder( *test ) / getResultName( *test )
				, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			m_currentRes = std::max( Source, m_currentRes );
			loadRes( m_currentRes );
		}
		else
		{
			m_currentRef = Source;
			m_currentRes = None;
			loadRes( m_currentRes );
		}

		m_refImage = loadRefImage( m_config.test, *test );
		m_currentRef = std::max( Source, m_currentRef );
		loadRef( m_currentRef );
	}

	void TestResultsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
	}

	void TestResultsPanel::loadRef( int index )
	{
		switch ( index )
		{
		case Source:
			m_ref->setImage( m_refImage );
			break;
		case Diff:
			m_ref->setImage( m_refToResImage );
			break;
		default:
			m_result->setImage( {} );
			index = None;
			break;
		}

		m_currentRef = ImgIndex( index );
	}

	void TestResultsPanel::loadRes( int index )
	{
		switch ( index )
		{
		case Source:
			m_result->setImage( m_resImage );
			break;
		case Diff:
			m_result->setImage( m_resToRefImage );
			break;
		default:
			m_result->setImage( {} );
			index = None;
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
