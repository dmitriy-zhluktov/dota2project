(function ($) {

/**
 * Gets the date format and time zone related parameters from PHP and formats
 * the date according to the PHP date formatters. Visit http://php.net/date for
 * more information. Using jQuery Timers
 * (http://plugins.jquery.com/project/timers) the date is recalculated every
 * second to make the clock dynamic.
 */
Drupal.behaviors.clockDisplay = {
  attach: function (context, settings) {

    // Create an array containing month names.
    var monthNames = new Array(
      // The 0 key is unneeded.
      {},
      {long: Drupal.t('January'), short: Drupal.t('Jan')},
      {long: Drupal.t('February'), short: Drupal.t('Feb')},
      {long: Drupal.t('March'), short: Drupal.t('Mar')},
      {long: Drupal.t('April'), short: Drupal.t('Apr')},
      // Use context if http://drupal.org/node/488496 gets in.
      {long: Drupal.t('May'), short: Drupal.t('May')},
      {long: Drupal.t('June'), short: Drupal.t('Jun')},
      {long: Drupal.t('July'), short: Drupal.t('Jul')},
      {long: Drupal.t('August'), short: Drupal.t('Aug')},
      {long: Drupal.t('September'), short: Drupal.t('Sep')},
      {long: Drupal.t('October'), short: Drupal.t('Oct')},
      {long: Drupal.t('November'), short: Drupal.t('Nov')},
      {long: Drupal.t('December'), short: Drupal.t('Dec')}
    );

    // Create an array containing weekday names.
    var weekdayNames = new Array(
      {long: Drupal.t('Sunday'), short: Drupal.t('Sun')},
      {long: Drupal.t('Monday'), short: Drupal.t('Mon')},
      {long: Drupal.t('Tuesday'), short: Drupal.t('Tue')},
      {long: Drupal.t('Wednesday'), short: Drupal.t('Wed')},
      {long: Drupal.t('Thursday'), short: Drupal.t('Thu')},
      {long: Drupal.t('Friday'), short: Drupal.t('Fri')},
      {long: Drupal.t('Saturday'), short: Drupal.t('Sat')}
    );

    var allSettings = Drupal.settings['clock'];
    var clocks = new Array();
    for (var i = 0; i < allSettings['names'].length; i++) {
      var cid = allSettings['names'][i];
      var settings = allSettings['clocks'][cid];

      // Initialize variables.
      // A PHP date format string.
      var dateFormat = settings['date_format'];
      // Prepare the timezone object.
      var timeZone = {
        // The name of the timezone, e.g. 'Europe/London'.
        name: settings['time_zone'],
        // The time zone offset in seconds.
        offsetSeconds: settings['offset_seconds'],
        // Daylight Savings Time information. '1' for yes, '0' for no.
        daylightSavingsTime: settings['daylight_savings_time'],
        // The name of the offset, e.g. 'GMT'.
        offsetName: settings['offset_name']
      };
      // Creates a JavaScript date object, by calculating the difference between
      // the target offset and the current offset and adding that to the current
      // time.
      // Note that due to JavaScript's inferior time zone handling, time zone
      // related date formatters will return the time zone of the Drupal site, not
      // the visiting user if the time zone is set to 'Local'.
      var date = new Date();
      // JavaScript returns the time zone offset reversely signed as PHP,
      // therefore we calculate the difference in the absolute values by adding
      // the two numbers.
      if (!settings['local']) {
        date = new Date(date.getTime() + (timeZone.offsetSeconds/60 + date.getTimezoneOffset())*60000);
      }
      
      clocks[cid] = {
        date: date,
        dateFormat: dateFormat,
        timeZone: timeZone
      }
    }

    var cid = allSettings['names'][0];
    clock = clocks[cid];
    $('span.' + cid).each(function () {
      $(this).once(cid, function() {
        window.setInterval(function () {
          for (var i = 0; i < allSettings['names'].length; i++) {
            cid = allSettings['names'][i];
            clock = clocks[cid];
            var timestamp = clock.date.getTime();
            timestamp += 1000;
            clock.date = new Date(timestamp);
            formattedDate = formatDate(clock.date, clock.dateFormat, clock.timeZone, monthNames, weekdayNames);
            $('span.' + cid).text(formattedDate);
            clocks[cid] = clock;
          };
        }, 1000);
      });
    });

  }
};

})(jQuery);

/**
 * Formats a date into a PHP date string.
 *
 * @param date
 *   A date object.
 * @param dateFormat
 *   A string. See http://php.net/date for possible values.
 * @param timezone
 *   An associative array containing timezone information. It should consist of
 *   following keys:
 *   - timezoneName: The name of the timezone, e.g. 'Europe/London'.
 *   - offsetSeconds: The timezone offset in seconds.
 *   - offsetName: The name of the offset, e.g. 'UTC'.
 *   - daylightSavingsTime: Whether or not the time is in daylight savings time.
 *     '1' if yes, '0' if not.
 * @param monthNames
 *   An array of month names keyed by the numbers 1-12. Each value should in turn
 *   be an array keyed 'long' and 'short' for the long respective the short
 *   month names.
 * @param weekdayNames
 *   An array of weekday names keyed by the numbers 0 (for Sunday) - 6 (for
 *   Saturday). Each value should in turn be an array keyed by 'long' and
 *   'short' for the long respective the short weekday names.
 *
 * @return
 *   A formatted date string.
 */
function formatDate(date, dateFormat, timeZone, monthNames, weekdayNames) {
  var timezoneName = timeZone.name;
  var offsetSeconds = timeZone.offsetSeconds;
  var offsetName = timeZone.offsetName;
  var daylightSavingsTime = timeZone.daylightSavingsTime;

  // Initialize the 'formattedDate' variable for later use.
  var formattedDate = '';

  // Prepare variables for the format conversion.

  // Year-related.
  var year = date.getFullYear();
  var yearShort = year % 100;
  // Calculate whether it is a leap year or not. Years that are multiples of 4
  // are leap years, while multiples of 100 are not, but multiples of 400 are.
  var year = date.getFullYear();
  if (((year % 4) == 0) && ((year % 100) != 0) || ((year % 400) == 0)) {
    var leapYear = '1';
  }
  else {
    var leapYear = '0';
  }

  // Month-related.
  var month = date.getMonth();
  
  // JavaScript starts counting the months with 0 instead of 1.
  month++;
  var monthLeadingZero = ((month < 10) ? '0' + month : month);
  var monthLongName = monthNames[month].long;
  var monthShortName = monthNames[month].short;
  var day = date.getDate();
  var dayLeadingZero = ((day < 10) ? '0' + day : day);
  switch (day) {
    case 1:
    case 21:
    case 31:
      var dayOfMonthAppend = Drupal.t('st');
      break;
    case 2:
    case 22:
      var dayOfMonthAppend = Drupal.t('nd');
      break;
    case 3:
    case 23:
      var dayOfMonthAppend = Drupal.t('rd');
      break;
    default:
      var dayOfMonthAppend = Drupal.t('th');
      break;
  }
  // Create an array containing month names and lengths.
  var months = new Array(
    0, // The 0 key is unneeded.
    31, // January
    ((leapYear == 1) ? 29 : 28), // February
    31, // March
    30, // April
    31, // May
    30, // June
    31, // July
    31, // August
    30, // September
    31, // Oktober
    30, // November
    31 // Dezember
  );
  // To calculate the days of the year, iterate through all passed months and then add the current days of the month.
  var daysOfYear = 0;
  for (var m = 1; m < month; m++) {
    daysOfYear += months[m];
  }
  daysOfYear += day;

  // Week-related.
  var weekday = date.getDay();
  if (weekday == 0) {
    var weekNumber = Math.floor((daysOfYear - 7) / 7) + 1;
  }
  else {
    var weekNumber = Math.floor((daysOfYear - weekday) / 7) + 1;
  }
  var weekNumberLeadingZero = ((weekNumber < 10) ? '0' + weekNumber : weekNumber);
  var weekdayLongName = weekdayNames[weekday].long;
  var weekdayShortName = weekdayNames[weekday].short;

  // Offset-related.
  if (offsetSeconds < 0) {
    var offsetPrefix = '-';
    var offsetHours = Math.floor(parseInt(offsetSeconds.substr(1)) / 3600);
    var offsetHoursLeadingZero = ((offsetHours < 10) ? '0' + offsetHours : offsetHours);
    var offsetMinutes = (parseInt(offsetSeconds.substr(1)) / 60) % 60;
    var offsetMinutesLeadingZero = ((offsetMinutes < 10) ? '0' + offsetMinutes : offsetMinutes);
  }
  else {
    var offsetPrefix = '+';
    var offsetHours = Math.floor(offsetSeconds) / 3600;
    var offsetHoursLeadingZero = ((offsetHours < 10) ? '0' + offsetHours : offsetHours);
    var offsetMinutes = (offsetSeconds / 60) % 60;
    var offsetMinutesLeadingZero = ((offsetMinutes < 10) ? '0' + offsetMinutes : offsetMinutes);
  }

  // Hour-related.
  var hours = date.getHours();
  var hoursLeadingZero = ((hours < 10) ? '0' + hours : hours);
  if (hours == 0) {
    var hoursTwelve = (hours + 12);
  }
  else if (hours > 12) {
    var hoursTwelve = (hours - 12);
  }
  else {
    var hoursTwelve = hours;
  }
  var hoursTwelveLeadingZero = ((hoursTwelve < 10) ? '0' + hoursTwelve : hoursTwelve);

  // Minute-related.
  var minutes = date.getMinutes();
  var minutesLeadingZero = ((minutes < 10) ? '0' + minutes : minutes);

  // Second-related.
  var seconds = date.getSeconds();
  var secondsLeadingZero = ((seconds < 10) ? '0' + seconds : seconds);
  var beats = Math.floor(((hours * 3600) + (minutes * 60) + seconds - parseInt(offsetSeconds) + 3600) / 86400 * 1000);
  if (beats < 10) {
    var beatsLeadingZero = ((beats < 10) ? '00' + beats : beats);
  }
  else if (beats < 100) {
    var beatsLeadingZero = ((beats < 100) ? '0' + beats : beats);
  }
  else {
    var beatsLeadingZero = beats;
  }
  var timestamp = date.getTime();

  // Turn the date format string into an array so each character has its own key.
  var dateFormat = dateFormat.split('');

  // Perform the date format conversion for a character.
  for (i=0; i < dateFormat.length; i++) {
    switch (dateFormat[i]) {
      // If the escape character '\' is used, simply return the following character.
      case '\\':
        formattedDate += dateFormat[++i];
        break;
      // 'am' or 'pm' depending on the time of day.
      case 'a':
        formattedDate += ((hours >= 12) ? Drupal.t('pm') : Drupal.t('am'));
        break;
      // 'AM' or 'PM' depending on the time of day.
      case 'A':
        formattedDate += ((hours >= 12) ? Drupal.t('PM') : Drupal.t('AM'));
        break;
      // Swatch-Internet-Time.
      case 'B':
        formattedDate += beatsLeadingZero;
        break;
      // ISO-8601 date.
      case 'c':
        formattedDate += year + '-' + monthLeadingZero + '-' + dayLeadingZero + 'T' + hoursLeadingZero + ':' + minutesLeadingZero + ':' + secondsLeadingZero + offsetPrefix + offsetHoursLeadingZero + ':' + offsetMinutesLeadingZero;
        break;
      // Day of month with leading zero, '01' - '31'.
      case 'd':
        formattedDate += dayLeadingZero;
        break;
      // Short name of weekday, e.g. 'Sun'.
      case 'D':
        formattedDate += weekdayShortName;
        break;
      // Time zone name, e.g. 'Europe/London';
      case 'e':
        formattedDate += timezoneName;
        break;
      // Name of month, e.g. 'January'.
      case 'F':
        formattedDate += monthLongName;
        break;
      // Hours in 12-hour format, '1' - '12'.
      case 'g':
        formattedDate += hoursTwelve;
        break;
      // Hours in 24-hour format, '0' - '23'.
      case 'G':
        formattedDate += hours;
        break;
      // Hours in 12-hour format with leading zero, '01' - '12'.
      case 'h':
        formattedDate += hoursTwelveLeadingZero;
        break;
      // Hours in 24-hour format with leading zero, '00' - '23'.
      case 'H':
        formattedDate += hoursLeadingZero;
        break;
      // Minutes with leading zero, '00' - '59'.
      case 'i':
        formattedDate += minutesLeadingZero;
        break;
      // Daylight Savings Time, '1' or '0'
      case 'I':
        formattedDate += daylightSavingsTime;
        break;
      // Day of month, '1' - '31'
      case 'j':
        formattedDate += day;
        break;
      // Name of weekday, e.g. 'Sunday'.
      case 'l':
        formattedDate += weekdayLongName;
        break;
      // Leap year, '1' or '0'.
      case 'L':
        formattedDate += leapYear;
        break;
      // Number of month with leading zero, '01' - '12'
      case 'm':
        formattedDate += monthLeadingZero;
        break;
      // Short name of month, e.g. 'Jan'.
      case 'M':
        formattedDate += monthShortName;
        break;
      // Number of month, '1' - '12'.
      case 'n':
        formattedDate += month;
        break;
      // ISO-8601 weekday number, '1' - '7'.
      case 'N':
        formattedDate += weekday + 1;
        break;
      // ISO-8601 year number.
      // This has a different value from 'Y' in certain constellations at the
      // beginning or end of a year, but this is not implemented here.
      case 'o':
        formattedDate += year;
        break;
      // Time zone offset, e.g. '+1030'
      case 'O':
        formattedDate += offsetPrefix + offsetHoursLeadingZero + offsetMinutesLeadingZero;
        break;
      // Time zone offset, e.g. '+10:30'.
      case 'P':
        formattedDate += offsetPrefix + offsetHoursLeadingZero + ':' + offsetMinutesLeadingZero;
        break;
      // RFC-2822 date.
      case 'r':
        formattedDate += weekdayShortName + ', ' + dayLeadingZero + ' ' + monthShortName + ' ' + year + ' ' + hoursLeadingZero + ':' + minutesLeadingZero + ':' + secondsLeadingZero + ' ' + offsetPrefix + offsetHoursLeadingZero + offsetMinutesLeadingZero;
        break;
      // Seconds with leading zero.
      case 's':
        formattedDate += secondsLeadingZero;
        break;
      // Appendage for the day of month, e.g. 'st' if the day is '1'.
      case 'S':
        formattedDate += dayOfMonthAppend;
        break;
      // Total days of month, '28' - '31'.
      case 't':
        formattedDate += months[month];
        break;
      // Name of offset, e.g. 'GMT'.
      case 'T':
        formattedDate += offsetName;
        break;
      // Milliseconds since the begin of the UNIX-era.
      case 'u':
        formattedDate += timestamp;
        break;
      // Seconds since the begin of the UNIX-era.
      case 'U':
        formattedDate += timestamp / 1000;
        break;
      // Number of weekday, '0' for Sunday, ..., '6' for Saturday.
      case 'w':
        formattedDate += weekday;
        break;
      // ISO-8601 week number, '01' - '52'.
      case 'W':
        formattedDate += weekNumberLeadingZero;
        break;
      // Year, e.g. '2001'.
      case 'Y':
        formattedDate += year;
        break;
      // Short year, e.g. '01'.
      case 'y':
        formattedDate += yearShort;
        break;
      // Days of the year, '0' - '365'.
      // Since PHP starts counting the days of the year at '0', we need subtract one.
      case 'z':
        formattedDate += daysOfYear - 1;
        break;
      // Time zone offset in seconds.
      case 'Z':
        formattedDate += offsetSeconds;
        break;
      // If the character is not a date formatter, just add it to the output.
      default:
        formattedDate += dateFormat[i];
        break;
    }
  }
  return formattedDate;
}
