#include "TestStatsPanel.hpp"

#include "DiffImage.hpp"
#include "Aui/AuiDockArt.hpp"
#include "Aui/AuiTabArt.hpp"
#include "Aui/AuiToolBarArt.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"

#pragma warning( push )
#pragma warning( disable:4189 )
#pragma warning( disable:4201 )
#pragma warning( disable:4242 )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4388 )
#pragma warning( disable:4389 )
#pragma warning( disable:4458 )
#pragma warning( disable:4706 )
#pragma warning( disable:4800 )
#pragma warning( disable:5219 )
#pragma warning( disable:5245 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wsource-uses-openmp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-zero"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/charts/wxchartslegendctrl.h>
#include <wx/charts/wxchartstheme.h>
#include <wx/charts/wxlinechartctrl.h>
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
#pragma warning( pop )

namespace aria
{
	//*********************************************************************************************

	namespace stats
	{
		enum ID
		{
			eID_PAGES,
		};

		static constexpr int LabelHeight = 20;
		static constexpr int LegendWidth = 100;

		static wxPanel * createPanel( wxWindow * parent
			, wxString const & name
			, wxPoint const & position
			, wxChartsCategoricalData::ptr chartData
			, wxChartsLegendData legendData )
		{
			static const wxLineChartOptions chartOptions{ []()
				{
					auto result = *wxChartsDefaultTheme->GetLineChartOptions();
					result.GetGridOptions().GetYAxisOptions().SetExplicitStartValue( 0.0 );
					return result;
				}() };

			auto size = parent->GetClientSize();
			size.y /= 2;
			auto result = new wxPanel{ parent, wxID_ANY, position, size };
			result->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			result->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			auto labelPanel = new wxPanel{ result, wxID_ANY, {}, { size.x, LabelHeight } };
			labelPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			labelPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto label = new wxStaticText{ labelPanel, wxID_ANY, name, {}, { size.x - 5, LabelHeight } };
			label->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			label->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			wxBoxSizer * labelSizer{ new wxBoxSizer{ wxHORIZONTAL } };
			labelSizer->Add( label, wxSizerFlags{ 1 }.Border( wxLEFT, 5 ).Align( wxALIGN_CENTRE_VERTICAL ) );
			labelPanel->SetSizerAndFit( labelSizer );

			size.y -= LabelHeight;
			auto chartPanel = new wxPanel{ result, wxID_ANY, { 0, LabelHeight }, size };
			chartPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chartPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto legend = new wxChartsLegendCtrl{ chartPanel, wxID_ANY, legendData, {}, { LegendWidth, size.y } };
			legend->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			legend->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			auto chart = new wxLineChartCtrl{ chartPanel, wxID_ANY, chartData, wxCHARTSLINETYPE_STRAIGHT, chartOptions, { LegendWidth, 0 }, { size.x - LegendWidth, size.y } };
			chart->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chart->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			wxBoxSizer * chartSizer{ new wxBoxSizer{ wxHORIZONTAL } };
			chartSizer->Add( legend, wxSizerFlags{}.Expand() );
			chartSizer->Add( chart, wxSizerFlags{ 1 }.Expand() );
			chartPanel->SetSizerAndFit( chartSizer );

			wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
			sizer->Add( labelPanel, wxSizerFlags{}.Expand() );
			sizer->Add( chartPanel, wxSizerFlags{ 1 }.Expand() );
			result->SetSizerAndFit( sizer );

			return result;
		}

		static wxString getShortName( wxString platformName
			, wxString cpuName
			, wxString gpuName )
		{
			platformName.Replace( " or greater", "" );
			gpuName.Replace( "NVIDIA ", "" );
			gpuName.Replace( "AMD ", "" );
			cpuName.Replace( "AMD ", "" );
			cpuName.Replace( "Intel ", "" );
			cpuName.Replace( " Processor", "" );
			return platformName + +wxT( " - " ) + cpuName + wxT( " - " ) + gpuName;
		}

		static wxString getShortName( Host const & host )
		{
			return getShortName( host.platform->name, host.cpu->name, host.gpu->name );
		}
	}

	//*********************************************************************************************

	HostTestStatsPanel::HostTestStatsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, TestDatabase & database
		, Host const & host )
		: wxPanel{ parent, id, {}, size }
		, m_database{ database }
		, m_host{ host }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
	}

	void HostTestStatsPanel::refresh()
	{
		if ( m_totalPanel )
		{
			RemoveChild( m_hostPanel );
			delete m_hostPanel;
			m_hostPanel = nullptr;

			RemoveChild( m_totalPanel );
			delete m_totalPanel;
			m_totalPanel = nullptr;

			RemoveChild( m_framePanel );
			delete m_framePanel;
			m_framePanel = nullptr;
		}

		m_hostPanel = new wxPanel{ this };
		m_hostPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto platformDest = new wxStaticText{ m_hostPanel, wxID_ANY, "Platform: " + m_host.platform->name };
		platformDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto cpuDest = new wxStaticText{ m_hostPanel, wxID_ANY, "CPU:" + m_host.cpu->name };
		cpuDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto gpuDest = new wxStaticText{ m_hostPanel, wxID_ANY, "GPU: " + m_host.gpu->name };
		gpuDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * hostSizer{ new wxBoxSizer{ wxVERTICAL } };
		hostSizer->Add( platformDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->Add( cpuDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->Add( gpuDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->SetSizeHints( m_hostPanel );
		m_hostPanel->SetSizer( hostSizer );

		auto & testRun = **m_test;
		auto times = m_database.listTestTimes( *testRun.test
			, testRun.renderer
			, m_host
			, m_maxStatus );
		wxVector< wxDouble > total;
		wxVector< wxDouble > avg;
		wxVector< wxDouble > last;
		wxVector< wxString > cats;

		for ( auto & time : times )
		{
			cats.push_back( time.first.FormatISODate() + " " + time.first.FormatISOTime() );
			total.push_back( double( time.second.total.count() ) / 1000.0 );
			avg.push_back( double( time.second.avg.count() ) / 1000.0 );
			last.push_back( double( time.second.last.count() ) / 1000.0 );
		}

		auto size = GetClientSize();
		size.y /= 2;
		wxChartsDoubleDataset::ptr totalDataSet( new wxChartsDoubleDataset( _( "Total" ), total, " ms" ) );
		auto totalTimesData = wxChartsCategoricalData::make_shared( cats );
		totalTimesData->AddDataset( totalDataSet );
		wxChartsLegendData totalLegendData( totalTimesData->GetDatasets() );
		m_totalPanel = stats::createPanel( this, _( "Total times" ), {}, totalTimesData, totalLegendData );

		wxChartsDoubleDataset::ptr avgDataSet( new wxChartsDoubleDataset( _( "Average" ), avg, " ms" ) );
		wxChartsDoubleDataset::ptr lastDataSet( new wxChartsDoubleDataset( _( "Last" ), last, " ms" ) );
		auto frameTimesData = wxChartsCategoricalData::make_shared( cats );
		frameTimesData->AddDataset( avgDataSet );
		frameTimesData->AddDataset( lastDataSet );
		wxChartsLegendData frameLegendData( frameTimesData->GetDatasets() );
		m_framePanel = stats::createPanel( this, _( "Frame times" ), { 0, size.y }, frameTimesData, frameLegendData );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->Add( m_hostPanel, wxSizerFlags{ 0 }.Border( wxALL, 0 ).Expand() );
		sizer->Add( m_totalPanel, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		sizer->Add( m_framePanel, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void HostTestStatsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		refresh();
	}

	void HostTestStatsPanel::filterTests( TestStatus maxStatus )
	{
		m_maxStatus = maxStatus;
		refresh();
	}

	void HostTestStatsPanel::deleteRun( uint32_t runId )
	{
		refresh();
	}

	//*********************************************************************************************

	TestStatsPanel::TestStatsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, TestDatabase & database )
		: wxPanel{ parent, id, {}, size }
		, m_database{ database }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_pages = new wxAuiNotebook( this
			, stats::eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_pages->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pages->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_pages->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_pages
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 200, 200 )
			.CaptionVisible( false )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false ) );
		m_auiManager.Update();
	}

	TestStatsPanel::~TestStatsPanel()
	{
		m_auiManager.UnInit();
	}

	void TestStatsPanel::refresh()
	{
		Freeze();
		auto sel = m_pages->GetSelection();
		auto it = m_hostPages.find( sel );
		int32_t selHost{};

		if ( it != m_hostPages.end() )
		{
			selHost = it->second;
		}

		m_pages->DeleteAllPages();

		for ( auto host : m_hosts )
		{
			RemoveChild( host.second );
		}

		m_hosts.clear();
		m_hostPages.clear();

		auto & testRun = **m_test;
		auto hosts = m_database.listTestHosts( *testRun.test
			, testRun.renderer );
		auto size = m_pages->GetClientSize();
		int index = 0;
		sel = 0;

		for ( auto host : hosts )
		{
			auto hostPanel = new HostTestStatsPanel{ this, wxID_ANY, size, m_database, *host };
			hostPanel->setTest( *m_test );
			m_pages->AddPage( hostPanel, stats::getShortName( *host ) );
			m_hosts.emplace( host->id, hostPanel );
			m_hostPages.emplace( index, host->id );

			if ( selHost == host->id )
			{
				sel = index;
			}

			++index;
		}

		m_pages->SetSelection( size_t( sel ) );
		Thaw();
		m_auiManager.Update();
	}

	void TestStatsPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		refresh();
	}

	void TestStatsPanel::deleteRun( uint32_t hostId
		, uint32_t runId )
	{
		auto it = m_hosts.find( int32_t( hostId ) );
		assert( it != m_hosts.end() );
		it->second->refresh();
		m_auiManager.Update();
	}

	//*********************************************************************************************
}
