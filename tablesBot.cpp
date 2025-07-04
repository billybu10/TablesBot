#include "include/tabulate.hpp"
#include "keys/keys.h"
#include <dpp/dpp.h>

#include <sstream>
#include <vector>
    
int main() {
    dpp::cluster bot(DSC_TOKEN);
    bot.on_log(dpp::utility::cout_logger());
    
    bot.on_ready([&bot](const auto& event) {
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_custom, "Waiting to create new table!"));

        if (dpp::run_once<struct boot_t>()) {

            bot.global_bulk_command_create({
                    dpp::slashcommand("simpletable", "Create table with no formatting", bot.me.id)
                    .set_interaction_contexts({dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel})
                });
        }
    });

    
    


    bot.register_command("simpletable", [](const dpp::slashcommand_t& e) {
	        dpp::interaction_modal_response modal("simpletablemodal", "Please enter table data according to the pattern, go to new line to add next row");
	 
	            modal.add_component(
	                dpp::component()
	                    .set_label("Type in table data")
	                    .set_id("table_data_field")
	                    .set_type(dpp::cot_text)
	                    .set_placeholder("cell1|cell2|cell3|and so on...")
	                    .set_max_length(4000)
	                    .set_text_style(dpp::text_paragraph)
	            );
	 
	            /* Trigger the dialog box. All dialog boxes are ephemeral */
	            e.dialog(modal);
	});

    bot.on_form_submit([](const dpp::form_submit_t & event) {
	        /* For this simple example, we know the first element of the first row ([0][0]) is value type string.
	         * In the real world, it may not be safe to make such assumptions!
	         */

            if(event.custom_id == "simpletablemodal"){
                std::string tableData = std::get<std::string>(event.components[0].components[0].value);

                tabulate::Table simpleTable;

                std::stringstream tableDataStream(tableData);
                char del = '\n';

                char cellDelimiter = '|';
                std::string row, cell;
                while (getline(tableDataStream, row, del)){
                    std::stringstream rowDataStream(row);

                    tabulate::Table::Row_t rowCells;
                    

                    while (getline(rowDataStream, cell, cellDelimiter)){
                        rowCells.push_back(cell);
                    }

                    
                    simpleTable.add_row(rowCells);

                }

                dpp::message m;
                m.set_content("``` "+simpleTable.str()+" ```");
        
                
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