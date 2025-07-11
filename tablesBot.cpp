#include "include/tabulate.hpp"
#include "keys/keys.h"
#include <dpp/dpp.h>

#include <sstream>
#include <vector>
#include <string>


bool longest(const tabulate::Table::Row_t& lhs, const tabulate::Table::Row_t& rhs)
{
  return lhs.size() < rhs.size();	
}

void applyFontAlignment(tabulate::Table& table, std::string parameters){
    std::istringstream tableDataStream(parameters);
    std::string styleParameter;
    std::vector<std::vector<std::string>> structuredParams;
    while (std::getline(tableDataStream, styleParameter, '\n')){
        std::istringstream parameterDataStream(styleParameter);

        std::vector<std::string> structuredParam;

        std::string temp_element;

        while (std::getline(parameterDataStream, temp_element, ':')){
            structuredParam.push_back(temp_element);
        }

        structuredParams.push_back(structuredParam);
    }

    for (std::vector<std::string> param : structuredParams)
    {
        if(param[0].contains("cell")){
            if (param[3].contains("l"))
            {
                table[std::stoi(param[2])-1][std::stoi(param[1])-1].format().font_align(tabulate::FontAlign::left);
            }else if (param[3].contains("c"))
            {
                table[std::stoi(param[2])-1][std::stoi(param[1])-1].format().font_align(tabulate::FontAlign::center);
            }else if (param[3].contains("r"))
            {
                table[std::stoi(param[2])-1][std::stoi(param[1])-1].format().font_align(tabulate::FontAlign::right);
            }
        }else if (param[0].contains("row")){
            if (param[2].contains("l"))
            {
                table.row(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::left);
            }else if (param[2].contains("c"))
            {
                table.row(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::center);
            }else if (param[2].contains("r"))
            {
                table.row(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::right);
            }
            
            
        }else if (param[0].contains("column")){
            if (param[2].contains("l"))
            {
                table.column(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::left);
            }else if (param[2].contains("c"))
            {
                table.column(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::center);
            }else if (param[2].contains("r"))
            {
                table.column(std::stoi(param[1])-1).format().font_align(tabulate::FontAlign::right);
            }

        }
    }
    
}
    
int main() {
    dpp::cluster bot(DSC_TOKEN);
    bot.on_log(dpp::utility::cout_logger());
    
    bot.on_ready([&bot](const auto& event) {
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, "Waiting to create new table!"));

        if (dpp::run_once<struct boot_t>()) {

            bot.global_bulk_command_create({
                    dpp::slashcommand("simpletable", "Create table with no formatting", bot.me.id)
                    .set_interaction_contexts({dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel}),
                    dpp::slashcommand("formattedtable", "Create table with formatting", bot.me.id)
                    .set_interaction_contexts({dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel})
                });
        }
    });


    bot.register_command("simpletable", [](const dpp::slashcommand_t& e) {
	        dpp::interaction_modal_response modal("simpletablemodal", "Enter table data according to the pattern");
	 
	            modal.add_component(
	                dpp::component()
	                    .set_label("Type table data, add new line to add next row")
	                    .set_id("table_data_field")
	                    .set_type(dpp::cot_text)
	                    .set_placeholder("cell1|cell2|cell3|and so on...")
	                    .set_max_length(4000)
	                    .set_text_style(dpp::text_paragraph)
	            );
	            e.dialog(modal);
	});


    bot.register_command("formattedtable", [](const dpp::slashcommand_t& e) {
	        dpp::interaction_modal_response modal("formattedtablemodal", "Enter table data according to the pattern");
	 
	            modal.add_component(
	                dpp::component()
	                    .set_label("Type table data, add new line to add next row")
	                    .set_id("table_data_field")
	                    .set_type(dpp::cot_text)
	                    .set_placeholder("cell1|cell2|cell3|and so on...")
	                    .set_max_length(4000)
	                    .set_text_style(dpp::text_paragraph)
	            );
                modal.add_row();
                modal.add_component(
	                dpp::component()
	                    .set_label("Type font style, add new line to add new rule")
	                    .set_id("font_style_field")
	                    .set_type(dpp::cot_text)
	                    .set_placeholder("cell:x:y:b or i or ib\ncolumn/row:number:value")
	                    .set_max_length(4000)
	                    .set_text_style(dpp::text_paragraph)
	            );
                modal.add_row();
                modal.add_component(
	                dpp::component()
	                    .set_label("Enter font alignment, new line for new rule")
	                    .set_id("font_alignment_field")
	                    .set_type(dpp::cot_text)
	                    .set_placeholder("cell:x:y:l or c or r\ncolumn/row:number:value")
	                    .set_max_length(4000)
	                    .set_text_style(dpp::text_paragraph)
	            );

	            e.dialog(modal);
	});

    bot.on_form_submit([&bot](const dpp::form_submit_t & event) {
            if(event.custom_id == "simpletablemodal"){
                std::string tableData = std::get<std::string>(event.components[0].components[0].value);

                tabulate::Table simpleTable;

                std::stringstream tableDataStream(tableData);
                char del = '\n';

                char cellDelimiter = '|';
                std::string row, cell;

                int max_size = 0;
                std::vector<tabulate::Table::Row_t> rows = std::vector<tabulate::Table::Row_t>();

                while (getline(tableDataStream, row, del)){
                    std::stringstream rowDataStream(row);

                    tabulate::Table::Row_t rowCells;
                    

                    while (getline(rowDataStream, cell, cellDelimiter)){
                        rowCells.push_back(cell);
                    }

                    rows.push_back(rowCells);
                }

                max_size = std::max_element(rows.begin(), rows.end(), longest)->size();

                for (tabulate::Table::Row_t row : rows)
                {
                    for(int i = 0; i<(max_size-row.size()); i++){
                        row.push_back("");
                    }
                    simpleTable.add_row(row);
                }
                
                simpleTable.format().multi_byte_characters(true);

                dpp::message m;
                m.set_content("```\n"+simpleTable.str()+" ```");
        
                
                event.reply(m);

            }else if(event.custom_id == "formattedtablemodal"){
                std::string tableData = std::get<std::string>(event.components[0].components[0].value);

                tabulate::Table formattedTable;

                std::stringstream tableDataStream(tableData);
                char del = '\n';

                char cellDelimiter = '|';
                std::string row, cell;

                int max_size = 0;
                std::vector<tabulate::Table::Row_t> rows = std::vector<tabulate::Table::Row_t>();

                while (getline(tableDataStream, row, del)){
                    std::stringstream rowDataStream(row);

                    tabulate::Table::Row_t rowCells;
                    

                    while (getline(rowDataStream, cell, cellDelimiter)){
                        rowCells.push_back(cell);
                    }

                    rows.push_back(rowCells);
                }

                max_size = std::max_element(rows.begin(), rows.end(), longest)->size();

                for (tabulate::Table::Row_t row : rows)
                {
                    for(int i = 0; i<(max_size-row.size()); i++){
                        row.push_back("");
                    }
                    formattedTable.add_row(row);
                }
                
                applyFontAlignment(formattedTable, std::get<std::string>(event.components[2].components[0].value));

                formattedTable.format()
                .multi_byte_characters(true);

                dpp::message m;
                m.set_content("```\n"+formattedTable.str()+" ```");
                bot.log(dpp::loglevel::ll_debug ,formattedTable.str());
                
                event.reply(m);
            }
	        
	    });


        bot.on_guild_create([&bot](const dpp::guild_create_t& event){
            dpp::embed hello_embed = dpp::embed()
            .set_color(dpp::colors::orange_yellow)
            .set_title("Hello! I'm ready to create tables!")
            .set_author("TablesBot", "https://github.com/billybu10/TablesBot", "https://github.com/billybu10/TablesBot/TablesIcon.png")
            .set_description("This is discord bot designed to help with sending tables. To learn more, click the link above")
            .set_url("https://github.com/billybu10/TablesBot/README.md");
            bot.message_create(dpp::message(event.created.system_channel_id, hello_embed));
            
        });

    
    bot.start(dpp::st_wait);
}