#include "TestStatsPanel.hpp"

#include "DiffImage.hpp"

#include <AriaLib/Aui/AuiDockArt.hpp>
#include <AriaLib/Aui/AuiTabArt.hpp>
#include <AriaLib/Aui/AuiToolBarArt.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/charts/wxchartslegendctrl.h>
#include <wx/charts/wxchartstheme.h>
#include <wx/charts/wxlinechartctrl.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	//*********************************************************************************************

	namespace stats
	{
		enum ID
		{
			eID_PAGES,
			eID_GRAPH_PAGES,
			eID_FILTER,
		};

		static wxWindow * createPanel( wxWindow * parent
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
			auto chart = new wxLineChartCtrl{ parent, wxID_ANY, chartData, wxCHARTSLINETYPE_STRAIGHT, chartOptions, {}, size };
			chart->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			chart->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

			return chart;
		}

		static wxString getShortName( wxString platformName
			, wxString cpuName
			, wxString gpuName )
		{
			platformName.Replace( " or greater", "" );
			gpuName.Replace( "NVIDIA ", "" );
			gpuName.Replace( "AMD ", "" );
			cpuName.Replace( "Intel ", "" );
			cpuName.Replace( " Processor", "" );
			return platformName + wxT( " - " ) + cpuName + wxT( " - " ) + gpuName;
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

		wxArrayString filterChoices;
		filterChoices.push_back( makeWxString( getName( TestStatus::eNegligible ) ) );
		filterChoices.push_back( makeWxString( getName( TestStatus::eAcceptable ) ) );
		filterChoices.push_back( makeWxString( getName( TestStatus::eUnacceptable ) ) );

		m_hostPanel = new wxPanel{ this };
		m_hostPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_platformDest = new wxStaticText{ m_hostPanel, wxID_ANY, "Platform: " + m_host.platform->name };
		m_platformDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_cpuDest = new wxStaticText{ m_hostPanel, wxID_ANY, "CPU:" + m_host.cpu->name };
		m_cpuDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_gpuDest = new wxStaticText{ m_hostPanel, wxID_ANY, "GPU: " + m_host.gpu->name };
		m_gpuDest->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * hostSizer{ new wxBoxSizer{ wxVERTICAL } };
		hostSizer->Add( m_platformDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->Add( m_cpuDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->Add( m_gpuDest, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		hostSizer->SetSizeHints( m_hostPanel );
		m_hostPanel->SetSizer( hostSizer );

		auto filterTxt = new wxStaticText{ this, wxID_ANY, "Filter: " };
		filterTxt->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto filterCombo = new wxComboBox{ this, stats::eID_FILTER, getName( m_maxStatus ), wxDefaultPosition, wxDefaultSize, filterChoices, wxCB_READONLY };
		filterCombo->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		filterCombo->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		wxBoxSizer * filterSizer{ new wxBoxSizer{ wxHORIZONTAL } };
		filterSizer->Add( filterTxt, wxSizerFlags{ 1 }.Border( wxALL, 0 ) );
		filterSizer->Add( filterCombo, wxSizerFlags{ 1 }.Border( wxALL, 0 ) );
		m_pages = new wxAuiNotebook( this
			, stats::eID_GRAPH_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_pages->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pages->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_pages->SetArtProvider( new AuiTabArt );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->Add( m_hostPanel, wxSizerFlags{ 0 }.Border( wxALL, 0 ).Expand() );
		sizer->Add( filterSizer, wxSizerFlags{ 0 }.Border( wxALL, 0 ) );
		sizer->Add( m_pages, wxSizerFlags{ 1 }.Border( wxALL, 0 ).Expand() );
		sizer->SetSizeHints( this );
		SetSizer( sizer );

		Bind( wxEVT_COMBOBOX
			, [this]( wxCommandEvent & evt )
			{
				m_maxStatus = TestStatus( evt.GetInt() + 1u );
				refresh();
			}
			, stats::eID_FILTER );
	}

	void HostTestStatsPanel::refresh()
	{
		Freeze();
		auto sel = m_pages->GetSelection();

		m_pages->DeleteAllPages();

		m_platformDest->SetLabel( "Platform: " + m_host.platform->name );
		m_cpuDest->SetLabel( "CPU:" + m_host.cpu->name );
		m_gpuDest->SetLabel( "GPU: " + m_host.gpu->name );

		auto & testRun = **m_test;
		auto times = m_database.listTestTimes( *testRun.test
			, testRun.renderer
			, m_host
			, m_maxStatus );
		std::array< wxString, 3u > names{ _( "Total" ), _( "Average" ), _( "Last" ) };
		std::array< wxVector< wxDouble >, 3u > catTimes;
		wxVector< wxString > cats;

		for ( auto & time : times )
		{
			cats.push_back( time.first.FormatISODate() + " " + time.first.FormatISOTime() );
			catTimes[0].push_back( double( time.second.total.count() ) / 1000.0 );
			catTimes[1].push_back( double( time.second.avg.count() ) / 1000.0 );
			catTimes[2].push_back( double( time.second.last.count() ) / 1000.0 );
		}

		for ( size_t i = 0u; i < catTimes.size(); ++i )
		{
			wxChartsDoubleDataset::ptr totalDataSet( new wxChartsDoubleDataset( names[i], catTimes[i], " ms" ) );
			auto totalTimesData = wxChartsCategoricalData::make_shared( cats );
			totalTimesData->AddDataset( totalDataSet );
			wxChartsLegendData totalLegendData( totalTimesData->GetDatasets() );
			auto totalPanel = stats::createPanel( this, {}, totalTimesData, totalLegendData );
			m_pages->AddPage( totalPanel, names[i] );
		}

		m_pages->SetSelection( size_t( sel ) );
		Thaw();
		Refresh();
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

	int HostTestStatsPanel::getSelection()
	{
		return m_pages->GetSelection();
	}

	void HostTestStatsPanel::setSelection( int sel )
	{
		m_pages->SetSelection( size_t( sel ) );
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

		std::map< int32_t, int > hostSel;

		for ( auto host : m_hosts )
		{
			hostSel.emplace( host.first, host.second->getSelection() );
		}

		m_pages->DeleteAllPages();

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

			auto hit = hostSel.find( host->id );

			if ( hit != hostSel.end() )
			{
				hostPanel->setSelection( hit->second );
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

		if ( it != m_hosts.end() )
		{
			it->second->refresh();
			m_auiManager.Update();
		}
	}

	//*********************************************************************************************
}
