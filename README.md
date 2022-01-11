# XIVNotifier

Final Fantasy XIV Login Queue notifier

Documentation is available in the following languages: [English](README.md) [Deutsch](README.de.md)

![Screenshot](screenshot2.png)

## Purpose

With the release of Endwalker, login queues are pain many times and last several hours. Maybe the same thing happened to you: You queued for login, saw 2500 people ahead of you, went for a walk, came back and found yourself in the intro cutsequence because you missed the login and got logged out again.

This tool allows you to get notified via messenger if the queue gets below a specific amount. Additionally it estaminetes the time how long the login process lasts.

## Installation and usage

* Download the latest version from [releases](https://github.com/vlohacks/XIVNotifier/releases/)
* Unzip the file where you want
* Run xivnotifier.exe
* Obtain CallMeBot API key and configure notification options in "Notifictation Settings". Details can be found in [Notice on Notification Feature](#notice-on-notification-feature)
* Run FF14, start login process
* Have some quality time while waiting for login :-)

## Notice on Notifictation Feature

XIVNotifier uses the CallMeBot API for notifying via Signal or WhatsApp Messenger. To use the feature, you need to create an API key for free and enter it in the program along with your mobile phone number. 

Details are here for [Whatsapp](https://www.callmebot.com/blog/free-api-whatsapp-messages/) and [Signal](https://www.callmebot.com/blog/free-api-signal-send-messages/)

## FAQ
Q: Why does the estaminated time take so long to show up?

A: This information is calculated from changes in the queue and the timespans between these changes. At least 3 changes are required. Just be patient.

Q: How does this generally work?

A: Just like a parser. It searches for the login queue amount in the game's memory and picks out the value.

Q: Help! The login count shows weird values and does not match the 

A: Congratulations, you found a bug. Please contact me :-). Until now the detection worked quite reliable, but if this happens a lot more, I have to improve it.

Q: I love you and want to give you tons of Gil ingame!

A: Visit me on Light/Shiva, Cynthia Cyber :-)

## Release information and disclaimer

This software comes without warrany, use it at your own responsibility.

