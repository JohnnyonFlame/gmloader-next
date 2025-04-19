#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

static const char* steam_stubs[] = {
    // Steam stubs
    // https://github.com/YoYoGames/GMEXT-Steamworks/wiki

    // Management
    "steam_init",
    "steam_update",
    "steam_shutdown",

    // General
    "steam_initialised",
    "steam_stats_ready",
    //"steam_get_app_id",
    //"steam_get_user_account_id",
    //"steam_get_user_steam_id",
    //"steam_get_persona_name",
    //"steam_get_user_persona_name",
    //"steam_get_user_persona_name_sync",
    //"steam_is_user_logged_on",
    //"steam_user_get_auth_ticket_for_web_api",
    //"steam_user_get_auth_session_ticket",
    //"steam_user_cancel_auth_ticket",
    "steam_current_game_language",
    //"steam_available_languages",
    //"steam_is_subscribed",
    //"steam_set_warning_message_hook",

    // Overlay
    "steam_is_overlay_enabled",
    "steam_is_overlay_activated",
    //"steam_activate_overlay",
    //"steam_activate_overlay_browser",
    //"steam_activate_overlay_store",
    //"steam_activate_overlay_user",
    //"steam_set_overlay_notification_inset",
    //"steam_set_overlay_notification_position",

    // Leaderboards
    //"steam_create_leaderboard",
    //"steam_upload_score",
    //"steam_upload_score_ext",
    //"steam_upload_score_buffer",
    //"steam_upload_score_buffer_ext",
    //"steam_download_scores",
    //"steam_download_scores_around_user",
    //"steam_download_friends_scores",
    //"steam_get_leaderboard_entry_count",
    //"steam_get_leaderboard_display_type",

    // Stats & Achievements
    "steam_set_achievement",
    "steam_get_achievement",
    "steam_clear_achievement",
    //"steam_indicate_achievement_progress",
    //"steam_get_achievement_progress_limits_int",
    //"steam_get_achievement_progress_limits_float",
    "steam_set_stat_int",
    "steam_set_stat_float",
    "steam_set_stat_avg_rate",
    "steam_get_stat_int",
    "steam_get_stat_float",
    "steam_get_stat_avg_rate",
    "steam_reset_all_stats",
    "steam_reset_all_stats_achievements",
    //"steam_request_global_stats",
    //"steam_request_global_achievement_percentages",
    //"steam_get_achievement_achieved_percent",
    //"steam_get_most_achieved_achievement_info",
    //"steam_get_next_most_achieved_achievement_info",
    //"steam_get_global_stat_int",
    //"steam_get_global_stat_real",
    //"steam_get_global_stat_history_int",
    //"steam_get_global_stat_history_real",
    //"steam_get_number_of_current_players",

    // Cloud
    //"steam_is_cloud_enabled_for_app",
    //"steam_is_cloud_enabled_for_account",
    //"steam_get_quota_total",
    //"steam_get_quota_free",
    //"steam_file_exists",
    //"steam_file_size",
    //"steam_file_persisted",
    //"steam_file_write",
    //"steam_file_write_file",
    //"steam_file_read",
    //"steam_file_read_buffer",
    //"steam_file_write_buffer",
    //"steam_file_share",
    //"steam_file_delete",
    //"steam_get_local_file_change",
    //"steam_get_local_file_change_count",
    //"steam_file_get_list",

    // UGC
    "steam_is_screenshot_requested",
    "steam_send_screenshot",

    // Social
    //"steam_set_rich_presence",
    //"steam_clear_rich_presence",
    //"steam_request_friend_rich_presence",
    //"steam_get_friend_rich_presence",
    //"steam_get_friend_rich_presence_key_count",
    //"steam_get_friend_rich_presence_key_by_index",
    //"steam_user_set_played_with",
    //"steam_get_friends_game_info",
    //"steam_get_user_avatar",
    //"steam_image_get_size",
    //"steam_image_get_rgba",
    //"steam_image_get_bgra",

    // Utils
    //"steam_show_floating_gamepad_text_input",
    //"steam_dismiss_floating_gamepad_text_input",
    //"steam_show_gamepad_text_input",
    //"steam_get_entered_gamepad_text_input",
    //"steam_utils_enable_callbacks",
    "steam_utils_is_steam_running_on_steam_deck",
    //"steam_utils_is_steam_in_big_picture_mode",
    //"steam_utils_set_game_launcher_mode",
    //"steam_utils_get_server_real_time",
    //"steam_utils_get_steam_ui_language"
};

ABI_ATTR void steam_stub(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0;
}

void patch_steam(struct so_module *mod)
{
    for (auto &func: steam_stubs)
        Function_Add(func, steam_stub, 1, 0);
}