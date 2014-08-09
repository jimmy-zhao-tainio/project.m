#include <lib.app/arguments-validate.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

bool arguments_validate (AppArgument *arguments)
{
        size_t i, k;
        size_t length;
        bool have_not_required = false;

        for (i = 0; arguments[i].type != AppArgumentTypeEnd; i++) {
                switch (arguments[i].value_type) {
                case AppArgumentBoolean:
                        if (!arguments[i].value.boolean) {
                                error (AppArgumentBooleanValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentInteger:
                        if (!arguments[i].value.integer) {
                                error (AppArgumentIntegerValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentUInt64:
                        if (!arguments[i].value.uint64) {
                                error (AppArgumentUInt64ValuePointerIsNull);
                                return false;
                        }
                        break;
                case AppArgumentString:
                        if (!arguments[i].value.string) {
                                error (AppArgumentStringValuePointerIsNull);
                                return false;
                        }
                        break;
                }
                if (arguments[i].type == AppArgumentTypeOrdinal) {
                        if (arguments[i].required) {
                                if (have_not_required) {
                                        error (AppArgumentOrdinalRequiredDiscontinuity);
                                        return false;
                                }
                        }
                        else {
                                have_not_required = true;
                        }
                        continue;
                }
                if (!arguments[i].object.named.short_form &&
                    !arguments[i].object.named.long_form) {
                        error (AppArgumentMissingShortOrLongForm);
                        return false;
                }
                if (arguments[i].object.named.short_form) {
                        if (string_length (arguments[i].object.named.short_form) != 2) {
                                error (AppArgumentInvalidShortFormLength);
                                return false;
                        }
                        if (arguments[i].object.named.short_form[0] != '-') {
                                error (AppArgumentShortFormMissingSingleDash);
                                return false;
                        }
                        if (arguments[i].object.named.short_form[1] == '-') {
                                error_code (AppArgumentInvalidShortFormName, 1);
                                return false;
                        }
                        if (!((arguments[i].object.named.short_form[1] >= '0' &&
                               arguments[i].object.named.short_form[1] <= '9') ||
                              (arguments[i].object.named.short_form[1] >= 'a' &&
                               arguments[i].object.named.short_form[1] <= 'z') ||
                              (arguments[i].object.named.short_form[1] >= 'A' &&
                               arguments[i].object.named.short_form[1] <= 'Z'))) {
                                error_code (AppArgumentInvalidShortFormName, 2);
                                return false;
                        }
                }
                if (arguments[i].object.named.long_form) {
                        if (!string_begins_with (arguments[i].object.named.long_form, "--")) {
                                error (AppArgumentLongFormMissingDoubleDash);
                                return false;
                        }
                        if ((length = string_length (arguments[i].object.named.long_form)) < 3) {
                                error (AppArgumentInvalidLongFormLength);
                                return false;
                        }
                        if (arguments[i].object.named.long_form[2] == '-') {
                                error_code (AppArgumentInvalidLongFormName, 1);
                                return false;
                        }
                        for (k = 2; k < length; k++) {
                                if (!((arguments[i].object.named.long_form[k] >= '0' &&
                                       arguments[i].object.named.long_form[k] <= '9') ||
                                      (arguments[i].object.named.long_form[k] == '-') ||
                                      (arguments[i].object.named.long_form[k] >= 'a' &&
                                       arguments[i].object.named.long_form[k] <= 'z') ||
                                      (arguments[i].object.named.long_form[k] >= 'A' &&
                                       arguments[i].object.named.long_form[k] <= 'Z'))) {
                                        error_code (AppArgumentInvalidLongFormName, 2);
                                        return false;
                                }
                        }
                }
                for (k = 0; arguments[k].type != AppArgumentTypeEnd; k++) {
                        if (arguments[k].type != AppArgumentTypeNamed) {
                                continue;
                        }
                        if (k == i) {
                                continue;
                        }
                        if (string_equals (arguments[k].object.named.short_form,
                                           arguments[i].object.named.short_form)) {
                                error (AppArgumentShortFormDuplicate);
                                return false;
                        }
                        if (string_equals (arguments[k].object.named.long_form,
                                           arguments[i].object.named.long_form)) {
                                error (AppArgumentLongFormDuplicate);
                                return false;
                        }
                }
        }
        return true;  
}
