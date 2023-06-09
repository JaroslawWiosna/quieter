#include "urgency.hpp"

float urgency_coefficient_prio(Priority p) {
    switch(p) {
        case Priority::MUST:
            return 6.0f;
        case Priority::SHOULD:
            return 3.9f;
        case Priority::COULD:
            return 1.8f;
        case Priority::WONT:
            return -3.9f;
        default:
            throw;
    } 
}

float urgency_due(int days) {
//    Datetime due (get_date ("due"));
//
//    // Map a range of 21 days to the value 0.2 - 1.0
//    float days_overdue = (now - due) / 86400.0;
//    if (days_overdue >= 7.0)   return 1.0;   // < 1 wk ago
//    else if (days_overdue >= -14.0) return ((days_overdue + 14.0) * 0.8 / 21.0) + 0.2;
//    else                            return 0.2;   // > 2 wks
//
    if (days <= -7) {
        return 1.0f;
    } else if (days >= 14) {
        return 0.2f;
    } else {
        return (-8.f / 210.f) * days + (22.f / 30.f);
    }
}
