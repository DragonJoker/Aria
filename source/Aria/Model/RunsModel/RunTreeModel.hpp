/*
See LICENSE file in root folder
*/
#ifndef ___CTP_RunTreeModel_HPP___
#define ___CTP_RunTreeModel_HPP___

#include "RunsModelPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/dataview.h>
#pragma warning( pop )

namespace aria::run
{
	/*
	RunTreeModel
	Implements the following data model:
		Status	RunDate		Platform		CPU			GPU			RunTime		TotalTime	AvgTime		LastTime
	-------------------------------------------------------------------------------------------------------------
	1:	bitmap	2020-11-06	PlatformName	CpuName		GpuName		15:35:22	20'000 ms	10 ms		10 ms
	*/
	class RunTreeModel
		: public wxDataViewModel
	{
	public:
		enum class Column
		{
			eStatus,
			eRunDateTime,
			ePlatformName,
			eCpuName,
			eGpuName,
			eTotalTime,
			eAvgTime,
			eLastTime,
			eCount,
		};

	public:
		RunTreeModel();
		~RunTreeModel()override;

		void addRun( Run run );
		RunTreeModelNode * getRunNode( uint32_t runId )const;
		void removeRun( uint32_t runId );
		void clear();
		void expandRoots( wxDataViewCtrl * view );
		void instantiate( wxDataViewCtrl * view );
		void resize( wxDataViewCtrl * view
			, wxSize const & size );

		// helper methods to change the model
		void deleteItem( wxDataViewItem const & item );

		// Custom comparison
		int Compare( wxDataViewItem const & item1
			, wxDataViewItem const & item2
			, unsigned int column
			, bool ascending )const override;

		// implementation of base class virtuals to define model
		unsigned int GetColumnCount()const override;
		wxString GetColumnType( unsigned int col )const override;
		void GetValue( wxVariant & variant
			, wxDataViewItem const & item
			, unsigned int col )const override;
		bool SetValue( wxVariant const & variant
			, wxDataViewItem const & item
			, unsigned int col ) override;
		wxDataViewItem GetParent( wxDataViewItem const & item )const override;
		bool IsContainer( wxDataViewItem const & item )const override;
		unsigned int GetChildren( wxDataViewItem const & parent
			, wxDataViewItemArray & array )const override;
		bool HasContainerColumns( const wxDataViewItem & item )const override;

	private:
		RunTreeModelNode * m_root;
	};
}

#endif
