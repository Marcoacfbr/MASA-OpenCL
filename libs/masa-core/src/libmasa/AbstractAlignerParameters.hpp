/*******************************************************************************
 *
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#ifndef ABSTRACTALIGNERPARAMETER_HPP_
#define ABSTRACTALIGNERPARAMETER_HPP_

#include <stdio.h>
#include <getopt.h>


/*
 * Used in the forkId parameter to indicates that the current process is not
 * a forked process
 */
#define NOT_FORKED_INSTANCE	(-1)

/**
 * The customized parameters of a MASA Extension, used for receiving,
 * manipulating and customizing command line parameters.
 *
 * Each MASA Extension must have its own subclass of the
 * AbstractAlignerParameters class. The subclass may customize
 * the command line parameters using the <a href="http://www.gnu.org/software/libc/manual/html_node/Getopt.html">GNU getopt C library</a>.
 * This is done by calling
 * the setArguments method for defining the option structure of the getopt
 * library and the usage string that will be displayed in the command line help.
 * The AbstractAlignerParameters::processArgument() pure virtual method will be
 * invoked whenever MASA encounters one of the customized parameters defined
 * with the setArguments method. Using this method, all the parameters may
 * be set as an attribute of the subclass.
 *
 * The aligner must set default values for each parameters because the
 * AbstractAlignerParameters::processArgument()
 * methods may never be called if there is no argument.
 *
 * The Aligner class must hold one instance of the AbstractAlignerParameters
 * class and it must return this instance in the IAlign::getParameters() method.
 *
 * One command line that the AbstractAlignerParameters manipulates is the
 * <tt>--fork</tt> parameter, since MASA use this parameter to fork as many
 * processes as the architecture supports. If MASA forks many processes,
 * the AbstractAlignerParameters::getForkId() method will return the
 * identifier of this forked process (from 0 to n-1), or NOT_FORKED_INSTANCE
 * if there was no forked process. Use this
 * Id to customize the execution uniquely to your architecture.
 *
 * @see AbstractAligner
 * @see <a href="http://www.gnu.org/software/libc/manual/html_node/Getopt.html">GNU getopt C library</a> (External link)
 */
class AbstractAlignerParameters {


public:
	/**
	 * Constructor
	 */
	AbstractAlignerParameters();

	/**
	 * Destructor
	 */
	virtual ~AbstractAlignerParameters();

	/**
	 * Returns the usage header to be shown in the command line help.
	 * @return the usage header.
	 */
	const char* getUsageHeader() const;

	/**
	 * Returns the usage text to be shown in the command line help. This
	 * string must explain all the parameters to the user.
	 *
	 * @return the usage text.
	 */
	const char* getUsage() const;

	/**
	 * Returns the option structure to be used in the getopt library function.
	 * See <a href="http://www.gnu.org/software/libc/manual/html_node/Getopt.html">GNU getopt C library</a>
	 * for a better comprehension of this structure.
	 * @return the option structure.
	 */
	const option* getArguments() const;


	/**
	 * Processes each customized command line option.
	 *
	 * For instance, suppose that the customized parameter was "--arg=OPT". The
	 * we will have this methos will be invoked with the following parameters:
	 * <ul>
	 *  <li>argument_str: the code associated with "--arg" in the option structure;
	 *  <li>argument_str: the "--arg=OPT" string;
	 *  <li>argument_option: the "OPT" string.
	 * </ul>
	 * Inside this method, the subclass should set the "arg" variable to OPT.
	 *
	 * @param argument_code The code of the argument associated in the
	 * 		AbstractAlignerParameters::setArguments method.
	 * @param argument_str The argument string with its option.
	 * @param argument_option The option passed to the argument. NULL if there
	 * 		is not any argument.
	 * @return zero if succeeded, non-zero otherwise. If there is an error,
	 * 		set the error string using the AbstractAlignerParameters::setLastError
	 * 		function.
	 */
	virtual int processArgument(int argument_code, const char* argument_str, const char* argument_option) = 0;

	/**
	 * Returns the last error defined by the AbstractAlignerParameters::setLastError
	 * method.
	 * @return the last error string.
	 */
	const char* getLastError() const;

	/**
	 * Returns the Id of the forked process.
	 *
	 * @return the Id of the forked process, from 0 to n-1 (where n is the
	 * number of forked process), or NOT_FORKED_INSTANCE (-1) if no process
	 * was forked.
	 */
	int getForkId() const;

	/**
	 * Defines the Id of the forked process.
	 * @param forkId the unique Id of this forked process.
	 */
	void setForkId(int forkId);


protected:

	/**
	 * Sets the arguments and the usage of the parameters.
	 *
	 * @param arguments	the option structure that will be passed to the getopt library.
	 * @param usage The string containing the help text that will be shown to the user.
	 * @param usageHeader The highlighted header of the help section that will contain
	 * 		the customized usage string.
	 */
	void setArguments(const option* arguments, const char* usage, const char* usageHeader);

	/**
	 * Defines the number of processes that may be forked using the "--fork"
	 * command line parameter.
	 *
	 * @param forkCount the maximum number of forked processes.
	 * @param forkWeights The weight of each processed used in the split procedure.
	 * 		set to NULL to consider equal weights.
	 */
	void setForkCount(const int forkCount, const int* forkWeights = NULL);

	/**
	 * Defines the error during the execution of the
	 * AbstractAlignerParameters::processArgument method.
	 *
	 * @param error The string containing the argument error.
	 */
	void setLastError(const char* error);


private:
	/** The last error */
	char lastError[500];

	/** The Id of this forked process */
	int forkId;

	/** Arguments passed to the getopt library */
	const option* arguments;

	/** Usage string */
	const char* usage;

	/** Usage section header */
	const char* usageHeader;

};

#endif /* ABSTRACTALIGNERPARAMETER_HPP_ */
