/*
 * DatasetFactory.h
 *
 *  Created on: 17 févr. 2014
 *      Author: etienne
 */

#pragma once

#include <climits>
#include <memory>
using namespace std;


#include "util/shortcuts.h"
#include "internals/transactions/IndexedTransactionsList.hpp"
#include "internals/tidlist/Template_ConsecutiveItemsConcatenatedTidList.hpp"
#include "internals/Counters.hpp"

using util::shp_array_int32;
using internals::transactions::IndexedTransactionsList;
using internals::tidlist::Template_ConsecutiveItemsConcatenatedTidList;

namespace internals {

/*
 * We use template to handle the appropriate item and tid size of the datasets.
 * This allows to generate very specialized code that the compiler will be able
 * to optimize heavily.
 * For example if a transaction is copied from the parent dataset to the new child
 * dataset and they have the same item size, then the compiler may replace the loop
 * with a more efficient operation (some kind of memcpy using SSE instructions).
 *
 * In the code below, we take into account the fact that the child item or tid size
 * cannot be bigger than the one of its parent. See the SAME_TYPES() tests below.
 * For example, if the parent item size is 16, then the child will be 8 or 16.
 * In this example, avoiding the case of 32 bits allows to reduce the code
 * generated with these templates.
 * */


#define SAME_TYPES(t1, t2) (sizeof(t1) == sizeof(t2))

/*
 * Dataset and DatasetFactoryImpl classes depend on each other.
 * Using the template type DatasetT allows to break
 * this dependency.
 * */

template <class DatasetT>
class DatasetFactoryImpl {

public:
	/*
	 * 1st step, detect the appropriate item size for the child dataset.
	 * ******************************************************************/
	template <class parentItemT, class parentTidT, class datasetInitializerT, typename... ArgsT>
	static unique_ptr<DatasetT> initDataset(
    		Counters *counters, ArgsT... args)
	{
		auto max_item = counters->maxFrequent;

		if (SAME_TYPES(parentItemT, uint8_t) || (max_item <= UINT8_MAX)) {
			return initDatasetWithItemT<parentTidT, datasetInitializerT, uint8_t, ArgsT...>(
					    		counters, args...);
		}

		if (SAME_TYPES(parentItemT, uint16_t) || (max_item <= UINT16_MAX)) {
			return initDatasetWithItemT<parentTidT, datasetInitializerT, uint16_t, ArgsT...>(
		    		counters, args...);
		}

		return initDatasetWithItemT<parentTidT, datasetInitializerT, uint32_t, ArgsT...>(
	    		counters, args...);

	}

	/*
	 * 2nd step, detect the appropriate tid size for the child dataset.
	 * ******************************************************************/
	template <class parentTidT, class datasetInitializerT, class itemT, typename... ArgsT>
	static unique_ptr<DatasetT> initDatasetWithItemT(
    			Counters *counters, ArgsT... args)
	{
		auto max_tid = counters->distinctTransactionsCount - 1;

		if (SAME_TYPES(parentTidT, uint8_t) || (max_tid <= UINT8_MAX)) {
			return initDatasetWithItemTandTidT<
					datasetInitializerT, itemT, uint8_t, ArgsT...>(
							counters, args...);
		}

		if (SAME_TYPES(parentTidT, uint16_t) || (max_tid <= UINT16_MAX)) {
			return initDatasetWithItemTandTidT<
					datasetInitializerT, itemT, uint16_t, ArgsT...>(
							counters, args...);
		}

		return initDatasetWithItemTandTidT<
				datasetInitializerT, itemT, uint32_t, ArgsT...>(
						counters, args...);
	}

	template <class datasetInitializerT, class itemT, class tidT, typename... ArgsT>
	static unique_ptr<DatasetT> initDatasetWithItemTandTidT(
						Counters *counters, ArgsT... args) {

		typedef IndexedTransactionsList<itemT> trnlist_t;
		typedef Template_ConsecutiveItemsConcatenatedTidList<tidT> tidlist_t;

		unique_ptr<trnlist_t> trnlist =
				unique_ptr<trnlist_t>(new trnlist_t(counters));
		unique_ptr<tidlist_t> tidlist =
				unique_ptr<tidlist_t>(new tidlist_t(counters, INT_MAX));

		datasetInitializerT::initialize(
				trnlist.get(), tidlist.get(), args...);

		return DatasetT::instanciateDataset(
				/* transfer ownership */
				std::move(trnlist),
				std::move(tidlist));
	}

	template <class parentDatasetT>
	struct ChildDatasetInitializer {

		template <class WriterT, class TidListT>
		static void initialize(
				WriterT *writer,
				TidListT *tidlist,
				parentDatasetT* parentDataset,
                int32_t extension,
                int32_t* renaming,
				int32_t max_candidate) {

			auto item_tidlist = parentDataset->getItemTidList(extension);
			parentDataset->copyTo(item_tidlist.get(), writer, tidlist,
				    		renaming, max_candidate);
		}
	};

	template <class parentDatasetT>
	static unique_ptr<DatasetT> initChildDataset(
				parentDatasetT* parentDataset,
                int32_t extension,
                int32_t* renaming,
	    		Counters *counters,
				int32_t max_candidate)
	{
		typedef ChildDatasetInitializer<parentDatasetT> initializerT;
		typedef typename parentDatasetT::item_type parentItemT;
		typedef typename parentDatasetT::tid_type parentTidT;

		return initDataset<parentItemT, parentTidT,
				initializerT, parentDatasetT*, int32_t, int32_t*, int32_t>(
				  		counters,
				  		parentDataset,
				  		extension,
				  		renaming,
				  		max_candidate);
	}

	template <class CopyableTransactionsListT>
	struct FirstDatasetInitializer {

		template <class WriterT, class TidListT>
		static void initialize(
				WriterT *writer,
				TidListT *tidlist,
				CopyableTransactionsListT* item_transactions,
				shp_array_int32 renaming,
				int32_t max_candidate) {

			item_transactions->copyTo(writer, tidlist, renaming->array, max_candidate);
		}
	};

	template <class CopyableTransactionsListT>
	static unique_ptr<DatasetT> initFirstDataset(
	    		CopyableTransactionsListT* item_transactions,
	    		shp_array_int32 renaming,
	    		Counters *counters,
	    		int32_t max_candidate)
	{
		typedef FirstDatasetInitializer<CopyableTransactionsListT> initializerT;
		return initDataset<uint32_t, uint32_t, initializerT, CopyableTransactionsListT*, shp_array_int32, int32_t>(
				  		counters,
				  		item_transactions,
				  		renaming,
				  		max_candidate);
	}
};

} /* namespace internals */

