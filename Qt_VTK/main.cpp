#include "mainwindow.h"
#include <QApplication>
#include <QIcon>


/**
 * @file main.cpp
 * @brief The entry point of the application.
 *
 * Initializes the QApplication object, creates the main window, and enters the application's main event loop.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return Returns the exit code of the application.
 */
int main(int argc, char* argv[])
{
	QApplication a(argc, argv); // Create the QApplication instance.

	MainWindow w; // Create the main window.


	w.setWindowIcon(QIcon(":/Downloads/logo.png"));

	// Set the window title
	w.setWindowTitle("VR Model Viewer");

	w.show(); // Show the main window.

	return a.exec(); // Enter the main event loop and wait until exit() is called.
}

