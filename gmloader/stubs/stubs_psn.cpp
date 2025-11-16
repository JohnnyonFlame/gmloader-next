#include "platform.h"
#include "so_util.h"
#include "libyoyo.h"

static const char *psn_stubs[] = {
    "psn_show_error_dialog",
    "psn_check_free_space",
    "psn_get_leaderboard_score_range",
    "psn_default_user_name",
    "psn_name_for_pad",
    "psn_unlock_trophy",
    "psn_get_trophy_unlock_state",
    "psn_init_np_libs",
    "psn_exit_np_libs",
    "psn_get_leaderboard_score",
    "psn_post_leaderboard_score",
    "psn_post_leaderboard_score_comment",
    "psn_check_np_availability",
    "psn_tick_error_dialog",
    "psn_tick",
    "psn_np_status",
    "psn_get_friends_scores",
    "psn_name_for_user",
    "psn_default_user",
    "psn_user_for_pad",
};

ABI_ATTR void psn_stub(RValue *ret, void *self, void *other, int argc, RValue *args)
{
    ret->kind = VALUE_REAL;
    ret->rvalue.val = 0;
}

void patch_psn(struct so_module *mod)
{
    for (auto &func: psn_stubs)
        Function_Add(func, psn_stub, 1, 0);
}