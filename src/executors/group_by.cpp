#include "global.h"
/**
 * @brief 
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> RETURN MAX|MIN|SUM|AVG(<attribute>)
 */
bool syntacticParseGROUP_BY()
{
    logger.log("syntacticParseGROUP_BY");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[1] != "<-" || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM"
			|| tokenizedQuery[7] != "RETURN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUP_BY;
	parsedQuery.groupByResultantRelationName = tokenizedQuery[0] ;
	parsedQuery.groupByRelationName = tokenizedQuery[6] ;
	parsedQuery.groupByGroupingAttribute = tokenizedQuery[4] ;
	
	string last_word = tokenizedQuery[8] ;
	switch(last_word.substr(0, 3)[2])
	{
		case 'X':
			parsedQuery.groupByAggregateFunction = MAX ; break ;
		case 'N':
			parsedQuery.groupByAggregateFunction = MIN ; break ;
		case 'G':
			parsedQuery.groupByAggregateFunction = AVG ; break ;
		case 'M':
			parsedQuery.groupByAggregateFunction = SUM ; break ;
		default:
			cout << "SYNTAX ERROR" << endl ;
			return false ;
	}

	parsedQuery.groupByAggregationAttribute = last_word.substr(4, last_word.size()-5);

    return true;
}

bool semanticParseGROUP_BY()
{
    logger.log("semanticParseGROUP_BY");

    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (tableCatalogue.isTable(parsedQuery.groupByResultantRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant Relation is already present." << endl;
        return false;
    }
	

	if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByGroupingAttribute, parsedQuery.groupByRelationName))
	{
		cout << "SEMANTIC ERROR: Grouping Attribute doesn't exist in relation" << endl;
		return false;
	}

	if(!tableCatalogue.isColumnFromTable(parsedQuery.groupByAggregationAttribute, parsedQuery.groupByRelationName))
	{
		cout << "SEMANTIC ERROR: Aggregation Attribute doesn't exist in relation" << endl;
		return false;
	}

    return true;
}

void executeGROUP_BY()
{
    logger.log("executeGROUP_BY");
	Table* table = tableCatalogue.getTable(parsedQuery.groupByRelationName) ;

	string agg_name ;
	switch(parsedQuery.groupByAggregateFunction)
	{
		case MAX:
			agg_name = "MAX" ; break ;
		case MIN:
			agg_name = "MIN" ; break ;
		case AVG:
			agg_name = "AVG" ; break ;
		case SUM:
			agg_name = "SUM" ; break ;
	}

	vector<string> resultantColumns = {parsedQuery.groupByGroupingAttribute, agg_name+parsedQuery.groupByAggregationAttribute};

	Table* resultantTable = new Table(parsedQuery.groupByResultantRelationName, resultantColumns) ;

	int grouping_index = table->getColumnIndex(parsedQuery.groupByGroupingAttribute) ;
	int aggregating_index = table->getColumnIndex(parsedQuery.groupByAggregationAttribute) ;

	Cursor cursor = table->getCursor() ;
	vector<int> row = cursor.getNext() ;

	map<int, int> aggregator ;
	map<int, int> class_count ;

	while(!row.empty())
	{
		int grouping_value = row[grouping_index] ;	
		int aggregation_value = row[aggregating_index] ;
		switch(parsedQuery.groupByAggregateFunction)
		{
			case SUM:
			case AVG:
				aggregator[grouping_value] += aggregation_value ;
				class_count[grouping_value]++ ;
				break ;
			case MIN:
				if(aggregator.find(grouping_value) == aggregator.end())
				{
					aggregator[grouping_value] = aggregation_value ;
				} else
					aggregator[grouping_value] = min(aggregator[grouping_value], aggregation_value) ;
				break ;

			case MAX:
				if(aggregator.find(grouping_value) == aggregator.end())
				{
					aggregator[grouping_value] = aggregation_value ;
				} else
					aggregator[grouping_value] = max(aggregator[grouping_value], aggregation_value) ;

		}	

		row = cursor.getNext() ;
	}

	for(const pair<int, int>& p : aggregator)
	{
		int key = p.first ;
		int val = p.second ;
		if(parsedQuery.groupByAggregateFunction == AVG)
		{
			val /= class_count[key] ;	
		}

		vector<int> row{key, val} ;
		resultantTable->writeRow(row) ;
	}

	if(resultantTable->blockify())
		tableCatalogue.insertTable(resultantTable) ;
	else
	{
		cout << "Empty Table" << endl ;
		delete resultantTable ;
	}
    return;
}
